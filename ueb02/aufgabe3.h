
#ifndef AUFGABE3_H
#define AUFGABE3_H

#define NUM_THREADS_3 5

extern int onbridge3;
extern int coll3;
extern char enter[NUM_THREADS_3];
extern int tickets[NUM_THREADS_3];

extern int lock3(long tid);
extern int unlock3(long tid);

extern void *driveOnBridge3();
extern int test3(void);

#endif
