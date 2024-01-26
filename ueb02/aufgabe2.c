#include "aufgabe2.h"
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int onbridge2;
int coll2;
char _lock[2];
int favoured;

int lock2 (long tid)
{
  _lock[tid] = 1;
  while (_lock[NUM_THREADS_2 - 1 - tid]) {
    if (favoured != tid) {
      _lock[tid] = 0;
      while (favoured != tid)
        ;
      _lock[tid] = 1;
    }
  }
  return 0;
}

int unlock2 (long tid)
{
  favoured = NUM_THREADS_2 - 1 - tid;
  _lock[tid] = 0;
  return 0;
}

void *driveOnBridge2(void *threadid) {
  long tid;
  tid = (long) threadid;

  // versuche in kritischen Zustand zu gehen
  lock2 (tid);
  // Anfang kritischer Zustand
  if (onbridge2) {
    coll2 = 1;
  }
  onbridge2 = 1; // Auto fährt auf die Brücke

  usleep(10);   // Zeit für die Überquerung der Brücke

  onbridge2 = 0; // Auto fährt von der Brücke runter
  // Ende kritischer Zustand

  // Austritt aus kritischen Zustand
  unlock2 (tid);

  pthread_exit(NULL);
}

int test2(void) {
  // initialisieren der Variablen
  pthread_t threads[NUM_THREADS_2]={0};
  int rc;

  onbridge2 = 0;
  coll2 = 0;
  _lock[0] = _lock[1] = 0;
  favoured = 1;

  // eigentlicher Ablauf der Simulation
  for (long i = 0; i < NUM_THREADS_2; i++) {
    rc = pthread_create (&threads[i], NULL, driveOnBridge2, (void *) i);
    if (rc) {
      // falls pthread_create nicht erfolgreich ist
      printf("ERROR; return code from pthread_create () is %d\n", rc);
      exit(1);
    }
    usleep(1);
  }
  for (int i = 0; i < NUM_THREADS_2; i++) {
    // warten bis alle Autos die Brücke überquert haben
    pthread_join(threads[i], NULL);
  }

  if (coll2) {
    printf("There were a collision.\n\n");
    return 1;
  } else {
    printf("There were not a collision.\n\n");
    return 0;
  }
}
