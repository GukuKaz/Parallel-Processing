#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sched.h>

#define BUFFER_SIZE 8
#define PRODUCER_COUNT 10
#define CONSUMER_COUNT 2

#define TRACE(...) do {           \
    fprintf(stderr, __VA_ARGS__); \
    putc('\n', stderr);           \
} while (0)

static inline void die(const char* msg) {
  perror(msg);
  exit(-1);
}

typedef struct {
  pthread_mutex_t lock;       // Monitor-Lock
  pthread_cond_t not_full;    // Condvar für Bedingung "nicht voll"
  pthread_cond_t not_empty;   // Condvar für Bedingung "nicht leer"
  size_t buffer[BUFFER_SIZE]; // Puffer
  size_t count;               // aktuelle Anzahl der Elemente im Puffer
  size_t produced;            // Gesamtanzahl produzierter Elemente
} Monitor_t;

// Initialisiert den Monitor.
void monitorInit(Monitor_t* self) {
  if (pthread_mutex_init(&self->lock, NULL) < 0)
    die("mutex_init()");
  
  if (pthread_cond_init(&self->not_full, NULL) < 0)
    die("cond_init()");
  
  if (pthread_cond_init(&self->not_empty, NULL) < 0)
    die("cond_init()");
  
  self->produced = self->count = 0;
}

// Gibt die Ressourcen im Monitor wieder frei.
void monitorRelease(Monitor_t* self) {
  pthread_mutex_destroy(&self->lock);
  pthread_cond_destroy(&self->not_full);
  pthread_cond_destroy(&self->not_empty);
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
  
  TRACE("Produzent %li legt Element %lu an Position %lu", pthread_self(), self->produced, self->count);
  
  // lege das neue Element in den Puffer (klappt garantiert!)
  self->buffer[self->count++] = self->produced++;
  
  // signalisiere potentiell wartenden Konsumenten, dass der Puffer Elemente hat
  pthread_cond_signal(&self->not_empty);
  pthread_mutex_unlock(&self->lock);
}

void monitorConsume(Monitor_t* self) {
  size_t res;
  
  pthread_mutex_lock(&self->lock);
  
  // teste ob der Puffer Elemente hat
  while (self->count == 0) {
    // falls nein, warte auf Signal durch monitorProduce()
    if (pthread_cond_wait(&self->not_empty, &self->lock) < 0)
      die("cond_wait()");
  }
  
  // entnehme das Element (klappt garantiert)
  res = self->buffer[--self->count];
  TRACE("Konsument %li entnimmt Element %lu von Position %lu", pthread_self(), res, self->count);
  
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

// Thread für Konsumenten.
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
