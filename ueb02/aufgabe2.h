
#ifndef AUFGABE2_H
#define AUFGABE2_H

#define NUM_THREADS_2   2

extern int onbridge2;
extern int coll2;
extern char _lock[2];
extern int favoured;

extern int lock2 (long tid);
extern int unlock2 (long tid);

extern void *driveOnBridge2();
extern int test2(void);

#endif
