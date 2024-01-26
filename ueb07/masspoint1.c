
#include <stdlib.h>
#include <math.h>
#include "masspoint1.h"

void move(masspoint masspntarr[], long massnum, double stepsize) {
  double acc[2] = {0,0};

  double relloc[2] = {0,0};
  double abs_relloc = 0;
  double G = 6.674*pow(10,-11);

  for (long a = 0; a < massnum; a++) {
    acc[0] = 0;
    acc[1] = 0;

    // calculate acceleration for masspoint a
    for (long i = 0; i < massnum; i++) {
      if (a != i) {
        relloc[0] = masspntarr[i].loc[0] - masspntarr[a].loc[0];
        relloc[1] = masspntarr[i].loc[1] - masspntarr[a].loc[1];

        abs_relloc = sqrt(relloc[0]*relloc[0] + relloc[1]*relloc[1]);

        acc[0] += masspntarr[i].mass * (relloc[0])/pow(abs_relloc,3);
        acc[1] += masspntarr[i].mass * (relloc[1])/pow(abs_relloc,3);
      } else {
        acc[0] += 0;
        acc[1] += 0;
      }
    }
    acc[0] *= G;
    acc[1] *= G;

    // calculate velocity for masspoint a
    masspntarr[a].vel[0] += acc[0] * stepsize;
    masspntarr[a].vel[1] += acc[1] * stepsize;
  }

  // calculate new locations
  for (long i = 0; i < massnum; i++) {
    masspntarr[i].loc[0] += masspntarr[i].vel[0] * stepsize;
    masspntarr[i].loc[1] += masspntarr[i].vel[1] * stepsize;
  }
}
