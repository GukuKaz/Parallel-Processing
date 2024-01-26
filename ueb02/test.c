
#include "aufgabe1.h"
#include "aufgabe2.h"
#include "aufgabe3.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char const *argv[]) {
  int count;
  int result1;
  int result2;
  int result3;

  if (argc > 1) {
    count = atoi(argv[1]);
  } else {
    count = 100000;
  }
  result1 = result2 = result3 = 0;
  printf("\nTest von Aufgabe 1\n");
  for (int i = 0; i < count; i++) {
    printf("Iteration: %d von %d\n", i+1, count);
    result1 += test1();
  }
  printf("\nTest von Aufgabe 2\n");
  for (int i = 0; i < count; i++) {
    printf("Iteration: %d von %d\n", i+1, count);
    result2 += test2();
  }
  printf("\nTest von Aufgabe 3\n");
  for (int i = 0; i < count; i++) {
    printf("Iteration: %d von %d\n", i+1, count);
    result3 += test3();
  }

  printf("Kollisionen in Aufgabe 1: %d\n", result1);
  printf("Kollisionen in Aufgabe 2: %d\n", result2);
  printf("Kollisionen in Aufgabe 3: %d\n", result3);
  return 0;
}
