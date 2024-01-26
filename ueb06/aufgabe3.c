#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sched.h>

#define BUFFER_SIZE 8
#define PRODUCER_COUNT 4
#define CONSUMER_COUNT 4
#define CONSUME_BATCH 4

#define TRACE(...) do {           \
    fprintf(stderr, __VA_ARGS__); \
    putc('\n', stderr);           \
} while (0)

static inline void die(const char* msg) {
  perror(msg);
  exit(-1);
}

typedef struct {
  pthread_mutex_t lock;                     // Monitor-Lock
  pthread_cond_t not_full;                  // Condvar für Bedingung "nicht voll"
  pthread_cond_t sufficient;                // Condvar für Bedingung "nicht leer"
  size_t buffer[BUFFER_SIZE];               // Puffer
  size_t count;                             // aktuelle Anzahl der Elemente im Puffer
  size_t produced, first, last;             // Gesamtanzahl produzierter Elemente
} Monitor_t;

// Initialisiert den Monitor.
void monitorInit(Monitor_t* self) {
  if (pthread_mutex_init(&self->lock, NULL) < 0)
    die("mutex_init()");
  
  if (pthread_cond_init(&self->not_full, NULL) < 0)
    die("cond_init()");
  
  if (pthread_cond_init(&self->sufficient, NULL) < 0)
    die("cond_init()");
  
  self->count = self->produced = self->first = self->last = 0;
}

// Gibt die Ressourcen im Monitor wieder frei.
void monitorRelease(Monitor_t* self) {
  pthread_mutex_destroy(&self->lock);
  pthread_cond_destroy(&self->not_full);
  pthread_cond_destroy(&self->sufficient);
}

// Produziert ein neues Element und legt es in den Puffer.
void monitorProduce(Monitor_t* self) {
  pthread_mutex_lock(&self->lock);
  
  // teste ob noch Platz im Puffer ist
  while (self->count >= BUFFER_SIZE) {
    // falls nein, warte auf Signal durch monitorConsume()
    if (pthread_cond_wait(&self->not_full, &self->lock) < 0)
      die("cond_wait()");
  }
  
  TRACE("Produzent %li legt Element %lu an Position %lu", pthread_self(), self->produced, self->last);
  
  //initialisieren der Zeiger im Buffer
  self->buffer[self->last] = self->produced++;
  self->last = (self->last + 1) % BUFFER_SIZE;
  ++self->count;
  
  // schicke Signal nur dann, wenn genug Elemente im Puffer sind
  if (self->count >= CONSUME_BATCH)
    pthread_cond_signal(&self->sufficient);
  
  pthread_mutex_unlock(&self->lock);
}

void monitorConsume(Monitor_t* self) {
  size_t res;
  pthread_mutex_lock(&self->lock);
  
  // teste ob genug Elemente im Puffer liegen
  while (self->count < CONSUME_BATCH) {
    // falls nein, warte auf Signal von monitorProduce()
    if (pthread_cond_wait(&self->sufficient, &self->lock) < 0)
      die("cond_wait()");
  }
  
  // Die feste belibiege abnahme der Elemente
  for (int i = 0; i < CONSUME_BATCH; ++i) {
    res = self->buffer[self->first];
    TRACE("Konsument %li entnimmt Element %lu von Position %lu", pthread_self(), res, self->first);
    self->first = (self->first + 1) % BUFFER_SIZE;
    --self->count;
  }
    // signalisiere den Konsumenten, dass wieder Platz im Puffer ist
  pthread_cond_signal(&self->not_full);
  pthread_mutex_unlock(&self->lock);
}

// Thread für Produzenten.
void* producer(void* data) {
  Monitor_t* monitor = data;
  
  while (1) {
    monitorProduce(monitor);
    //sched_yield();
  }
  
  return NULL;
}
// Thread für Konsumenten
void* consumer(void* data) {
  Monitor_t* monitor = data;
  
  while (1) {
    monitorConsume(monitor);
    //sched_yield();
  }
  
  return NULL;
}

int main() {
  pthread_t producers[PRODUCER_COUNT];
  pthread_t consumers[CONSUMER_COUNT];
  Monitor_t monitor;
  
  // gemeinsamen Monitor initialisieren
  monitorInit(&monitor);
  
    // Produzenten instanzieren und Monitor bekannt machen
  for (size_t i = 0; i < PRODUCER_COUNT; i++) {
    if (pthread_create(&producers[i], NULL, &producer, &monitor) < 0)
      die("pthread_create()");
  }
  
  // Konsumenten instanzieren und Monitor bekannt machen
  for (size_t i = 0; i < CONSUMER_COUNT; i++) {
    if (pthread_create(&consumers[i], NULL, &consumer, &monitor) < 0)
      die("pthread_create()");
  }
  
  // eine Sekunde ausführen lassen
  sleep(1);
  
  // Thread abbrechen
  for (size_t i = 0; i < PRODUCER_COUNT; i++) {
    pthread_cancel(producers[i]);
  }
  
  for (size_t i = 0; i < CONSUMER_COUNT; i++) {
    pthread_cancel(consumers[i]);
  }
  
  // Monitor freigeben (potentieller Segfault, da nicht auf Abbruch gewartet)
  monitorRelease(&monitor);
  return 0;
}
