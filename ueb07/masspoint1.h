#ifndef MASSPOINT_H
#define MASSPOINT_H 1

typedef struct {
  double mass;
  double loc[2];
  double vel[2];
} masspoint;

void move(masspoint masspntarr[], long massnum, double stepsize);

#endif
