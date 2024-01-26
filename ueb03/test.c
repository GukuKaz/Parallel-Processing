
#include "aufgabe1.h"
#include "aufgabe3.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

int main(int argc, char const *argv[]) {
  int count;
  int num_threads;
  int result1, result3;
  time_t time1;
  int timeresult1, timeresult2;

  if (argc == 2) {
    count = atoi(argv[1]);
    num_threads = 5;
  } else if (argc > 2) {
    count = atoi(argv[1]);
    num_threads = atoi(argv[2]);
  } else {
    count = 100000;
    num_threads = 5;
  }

  result1 = result3 = 0;
  printf("\nTest von Übungsblatt 2 Aufgabe 3 mit %d Threads\n", num_threads);
  time1 = time(NULL);
  for (int i = 0; i < count; i++) {
    printf("Iteration: %d von %d\n", i+1, count);
    result3 += test3(num_threads);
  }
  timeresult1 = time(NULL) - time1;

  printf("\nTest von Aufgabe 1 mit %d Threads\n", num_threads);
  time1 = time(NULL);
  for (int i = 0; i < count; i++) {
   printf("Iteration: %d von %d\n", i+1, count);
   result1 += test1(num_threads);
  }
  timeresult2 = time(NULL) - time1;

  printf("Kollisionen in Übungsblatt 2 Aufgabe 3 mit %d Threads: %d\n", num_threads, result3);
  printf("Zeit für Übungsblatt 2 Aufgabe 3 mit %d Threads: %d\n", num_threads, timeresult1);
  printf("Kollisionen in Aufgabe 1 mit %d Threads: %d\n", num_threads, result1);
  printf("Zeit für Aufgabe 1 mit %d Threads: %d\n", num_threads, timeresult2);
  pthread_exit(NULL);
}
