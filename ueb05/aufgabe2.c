
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define NUM_PLACES        10

sem_t empty;
sem_t mutex;

int actProducers = 0;
int last = 0;
int buffersize;
int *buffer;

int numPro;
int numCon;
int numThreads;

void* Producer (void *threadid) {
  int i;
  long tid = (long) threadid;

  // anmelden des Produzenten (gesichert falls mehrere Produzenten starten)
  sem_wait(&mutex);
  actProducers++;
  sem_post(&mutex);

  for (i = 0; i < 50; i++) {
    sem_wait(&mutex);
    // resizen falls Buffer mehr als zu 80% gefüllt
    if (last >= ((float) buffersize)*0.8) {
      buffersize *= 2;
      printf("resize\n");
      buffer = realloc(buffer, buffersize*sizeof(buffer));
    }

    buffer[last] = i;

    printf("Producer %ld puts %d into buffer at place %d \n", tid,
      buffer[last], last);
    last++;

    sem_post(&mutex);
    sem_post(&empty);
  }

  // abmelden des Produzenten (gesichert falls mehrere Produzenten beenden)
  sem_wait(&mutex);
  printf("Producer %ld exits \n", tid);
  actProducers--;
  sem_post(&mutex);

  pthread_exit(NULL);
}

void* Consumer (void *threadid) {
  long tid = (long) threadid - numPro;
  int emp;
  char test = 1;

  while (test) {
    emp = sem_trywait(&empty);      // versuch in empty zu kommen
    if (last>0 && !emp) {           // wenn buffer wirklich nicht leer ist
      sem_wait(&mutex);             // -> konsumiere

      last--;
      printf("Consumer %ld takes %d from buffer at place %d \n", tid,
        buffer[last], last);

      sem_post(&mutex);

    } else if (actProducers==0 && last == 0) {  // wenn keine Produzenten mehr da
      test=0;                                   // und Buffer leer
                                                // -> beende Schleife
    }
    sleep(0.1);                                 // einfach damit nicht so massiv
                                                // busy waiting passiert
  }

  printf("Consumer %ld exits\n", tid);
  pthread_exit(NULL);
}

int main(int argc, char const *argv[]) {
  if (argc == 1) {
    numPro = 1;
    numCon = 4;
  } else if (argc == 2) {
    numPro = atoi(argv[1]);
    numCon = 4;
  } else if (argc > 2) {
    numPro = atoi(argv[1]);
    numCon = atoi(argv[2]);
  }

  numThreads = numPro + numCon;

  pthread_t threads[numThreads];
  long t;
  buffersize = NUM_PLACES;
  buffer = malloc(buffersize * sizeof(buffer));

  sem_init(&mutex, 0, 1);
  sem_init(&empty, 0, 0);

  for (t = 0; t < numPro; t++) {
    pthread_create(&threads[t], NULL, Producer, (void *) t);
  }

  sleep(0.1);

  for (t = numPro; t < numThreads; t++) {
    pthread_create(&threads[t], NULL, Consumer, (void *) t);
  }

  for (t = 0; t < numThreads; t++) {
    pthread_join(threads[t], NULL);
  }

  sem_destroy(&mutex);
  sem_destroy(&empty);

  return 0;
}
