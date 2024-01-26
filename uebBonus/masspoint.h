#ifndef MASSPOINT_H
#define MASSPOINT_H 1

typedef struct {
  double mass;
  double loc[2];
  double vel[2];
} masspoint;

void calc_new_vel(masspoint masspntarr[], long massnum, double stepsize, long start, long stop);
void move(masspoint masspntarr[], double stepsize, long start, long stop);

#endif
