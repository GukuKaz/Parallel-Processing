#include "aufgabe1.h"
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int onbridge1;
int coll1;

void *driveOnBridge1() {

  if (onbridge1) {
    coll1 = 1;
  }
  onbridge1 = 1; // Auto fährt auf die Brücke

  usleep(10);   // Zeit für die Überquerung der Brücke

  onbridge1 = 0; // Auto fährt von der Brücke runter
  pthread_exit(NULL);
}

int test1(void) {
  // initialisieren und ggf. belegen (wenn nötig) der Variablen
  pthread_t threads[NUM_THREADS_1]={0};
  onbridge1 = 0;
  coll1 = 0;
  int rc;

  // eigentlicher Ablauf der Simulation
  for (int i = 0; i < NUM_THREADS_1; i++) {
    rc = pthread_create (&threads[i], NULL, driveOnBridge1, NULL);
    if (rc) {
      // falls pthread_create nicht erfolgreich ist
      printf("ERROR; return code from pthread_create () is %d\n", rc);
      exit(1);
    }
    usleep(1);
  }
  for (int i = 0; i < NUM_THREADS_1; i++) {
    // warten bis alle Autos die Brücke überquert haben
    pthread_join(threads[i], NULL);
  }

  // Auswärtung der Simulation
  // Unfälle zählen
  if (coll1) {
    printf("There were a collision.\n\n");
    return 1;
  } else {
    printf("There were not a collision.\n\n");
    return 0;
  }
}
