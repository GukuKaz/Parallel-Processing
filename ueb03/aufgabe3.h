
#ifndef AUFGABE3_H
#define AUFGABE3_H

extern int onbridge3;
extern int coll3;
extern char *enter;
extern int *tickets;

extern int lock3(long tid);
extern int unlock3(long tid);

extern void *driveOnBridge3();
extern int test3(int num_threads);

#endif
