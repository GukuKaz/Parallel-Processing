
#include "aufgabe1.h"
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int onbridge1;
int coll1;
pthread_mutex_t lock;

void *driveOnBridge1() {
  int _error = 0;

  // versuche in kritischen Zustand zu gehen
  _error = pthread_mutex_lock(&lock);
  // Anfang kritischer Zustand
  if (onbridge1) {
    coll1 = 1;
  }
  onbridge1 = 1; // Auto fährt auf die Brücke

  usleep(10);   // Zeit für die Überquerung der Brücke

  onbridge1 = 0; // Auto fährt von der Brücke runter
  // Ende kritischer Zustand

  // Austritt aus kritischen Zustand
  _error = pthread_mutex_unlock(&lock);

  pthread_exit(NULL);
}

int test1(int num_threads) {
  // initialisieren der Variablen
  int thread_anzahl = num_threads;
  pthread_t *threads = malloc(thread_anzahl * sizeof(threads));;
  int rc;

  pthread_mutex_init(&lock, NULL);

  onbridge1 = 0;
  coll1 = 0;

  // eigentlicher Ablauf der Simulation
  pthread_t *helpThreads = threads;
  for (long i = 0; i < thread_anzahl; i++) {
    rc = pthread_create (helpThreads, NULL, driveOnBridge1, NULL);
    if (rc) {
      // falls pthread_create nicht erfolgreich ist
      printf("ERROR; return code from pthread_create () is %d\n", rc);
      exit(1);
    }
    usleep(1);
    helpThreads++;
  }

  helpThreads = threads;
  for (int i = 0; i < thread_anzahl; i++) {
    // warten bis alle Autos die Brücke überquert haben
    pthread_join(*helpThreads, NULL);
    helpThreads++;
  }

  pthread_mutex_destroy(&lock);

  if (coll1) {
    printf("There were a collision.\n\n");
    return 1;
  } else {
    printf("There were not a collision.\n\n");
    return 0;
  }
}
