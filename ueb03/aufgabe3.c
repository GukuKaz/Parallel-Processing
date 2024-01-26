
#include "aufgabe3.h"
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int onbridge3;
int coll3;
char *enter;
int *tickets;
int thread_anzahl;

char getEnter(long stelle) {
  char *helpEnter = enter;
  for (int i = 0; i <= stelle; i++) {
    helpEnter++;
  }
  return *helpEnter;
}

void setEnter(char value, long stelle) {
  char *helpEnter = enter;
  for (int i = 0; i <= stelle; i++) {
    helpEnter++;
  }
  *helpEnter = value;
}

int getTickets(long stelle) {
  int *helpTickets = tickets;
  for (int i = 0; i <= stelle; i++) {
    helpTickets++;
  }
  return *helpTickets;
}

void setTickets(int value, long stelle) {
  int *helpTickets = tickets;
  for (int i = 0; i <= stelle; i++) {
    helpTickets++;
  }
  *helpTickets = value;
}

int lock3 (long tid)
{
  long i, max;

  setEnter(1, tid);
  max = 0;
  for (i = 0; i < thread_anzahl; i++) {
    if (max < getTickets(i)) {
      max = getTickets(i);
    }
  }
  setTickets(max+1, tid);
  setEnter(0, tid);

  for (i = 0; i < thread_anzahl; i++) {
    if (i != tid) {
      while (getEnter(i))
        ;
      while ((getTickets(i) != 0) &&
        ((getTickets(tid) > getTickets(i)) ||
        ((getTickets(tid) == getTickets(i)) &&
        (tid > i))))
        ;
    }
  }
  return 0;
}

int unlock3 (long tid)
{
  setTickets(0, tid);
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

int test3(int num_threads) {
  // initialisieren der Variablen
  thread_anzahl = num_threads;
  pthread_t *threads = malloc(thread_anzahl * sizeof(threads));
  enter = malloc(thread_anzahl * sizeof(enter));
  tickets = malloc(thread_anzahl * sizeof(tickets));

  int rc;

  onbridge3 = 0;
  coll3 = 0;

  // eigentlicher Ablauf der Simulation
  pthread_t *helpThreads = threads;
  for (long i = 0; i < thread_anzahl; i++) {
    rc = pthread_create (helpThreads, NULL, driveOnBridge3, (void *) i);
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

  if (coll3) {
    printf("There were a collision.\n\n");
    return 1;
  } else {
    printf("There were not a collision.\n\n");
    return 0;
  }
}
