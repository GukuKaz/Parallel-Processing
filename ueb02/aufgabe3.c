#include "aufgabe3.h"
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int onbridge3;
int coll3;
char enter[NUM_THREADS_3];
int tickets[NUM_THREADS_3];

int lock3 (long tid)
{
  int i, max;

  enter[tid] = 1;
  max = 0;
  for (i = 0; i < NUM_THREADS_3; i++) {
    if (max < tickets[i]) {
      max = tickets[i];
    }
  }
  tickets[tid] = max + 1;
  enter[tid] = 0;

  for (i = 0; i < NUM_THREADS_3; i++) {
    if (i != tid) {
      while (enter[i])
        ;
      while ((tickets[i] != 0) &&
        ((tickets[tid] > tickets[i]) ||
        ((tickets[tid] == tickets[i]) &&
        (tid > i))))
        ;
    }
  }
  return 0;
}

int unlock3 (long tid)
{
  tickets[tid] = 0;
  return 0;
}

void *driveOnBridge3(void *threadid) {
  long tid;
  tid = (long) threadid;

  // versuche in kritischen Zustand zu gehen
  lock3 (tid);
  // Anfang kritischer Zustand
  if (onbridge3) {
    coll3 = 1;
  }
  onbridge3 = 1; // Auto fährt auf die Brücke

  usleep(10);   // Zeit für die Überquerung der Brücke

  onbridge3 = 0; // Auto fährt von der Brücke runter
  // Ende kritischer Zustand

  // Austritt aus kritischen Zustand
  unlock3 (tid);

  pthread_exit(NULL);
}

int test3(void) {
  // initialisieren der Variablen
  pthread_t threads[NUM_THREADS_3]={0};
  int rc;

  onbridge3 = 0;
  coll3 = 0;

  // eigentlicher Ablauf der Simulation
  for (long i = 0; i < NUM_THREADS_3; i++) {
    rc = pthread_create (&threads[i], NULL, driveOnBridge3, (void *) i);
    if (rc) {
      // falls pthread_create nicht erfolgreich ist
      printf("ERROR; return code from pthread_create () is %d\n", rc);
      exit(1);
    }
    usleep(1);
  }
  for (int i = 0; i < NUM_THREADS_3; i++) {
    // warten bis alle Autos die Brücke überquert haben
    pthread_join(threads[i], NULL);
  }

  if (coll3) {
    printf("There were a collision.\n\n");
    return 1;
  } else {
    printf("There were not a collision.\n\n");
    return 0;
  }
}
