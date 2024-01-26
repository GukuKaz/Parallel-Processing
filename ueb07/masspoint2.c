
#include <stdlib.h>
#include <math.h>
#include <omp.h>
#include "masspoint2.h"

void move(masspoint masspntarr[], long massnum, double stepsize) {
  double acc[2] = {0,0};

  double relloc[2] = {0,0};
  double abs_relloc = 0;
  double G = 6.674*pow(10,-11);
  long a=0;
  long i=0;
  masspoint tmpfora;
  masspoint tmpfori;

  for (a = 0; a < massnum; a++) {
    acc[0] = 0;
    acc[1] = 0;
    tmpfora = masspntarr[a];

    //calculate acceleration for masspoint a
    #pragma omp parallel shared (masspntarr, massnum, tmpfora) private (i, abs_relloc, relloc, tmpfori)
    {
      //  in x
      #pragma omp for reduction (+: acc[0])
      for (i = 0; i < massnum; i++) {
        if (a != i) {
          tmpfori = masspntarr[i];
          relloc[0] = tmpfori.loc[0] - tmpfora.loc[0];
          relloc[1] = tmpfori.loc[1] - tmpfora.loc[1];

          abs_relloc = sqrt(relloc[0]*relloc[0] + relloc[1]*relloc[1]);

          acc[0] += tmpfori.mass * (relloc[0])/pow(abs_relloc,3);
        } else {
          acc[0] += 0;
        }
      }

      //  in y
      #pragma omp for reduction (+: acc[1])
      for (i = 0; i < massnum; i++) {
        if (a != i) {
          tmpfori = masspntarr[i];
          relloc[0] = tmpfori.loc[0] - tmpfora.loc[0];
          relloc[1] = tmpfori.loc[1] - tmpfora.loc[1];

          abs_relloc = sqrt(relloc[0]*relloc[0] + relloc[1]*relloc[1]);

          acc[1] += tmpfori.mass * (relloc[1])/pow(abs_relloc,3);
        } else {
          acc[1] += 0;
        }
      }
    }

    acc[0] *= G;
    acc[1] *= G;

    // calculate velocity for masspoint a
    masspntarr[a].vel[0] += acc[0] * stepsize;
    masspntarr[a].vel[1] += acc[1] * stepsize;
  }

  // calculate new locations
  #pragma omp parallel for shared (masspntarr, massnum, stepsize) private (i)
  for (i = 0; i < massnum; i++) {
    masspntarr[i].loc[0] += masspntarr[i].vel[0] * stepsize;
    masspntarr[i].loc[1] += masspntarr[i].vel[1] * stepsize;
  }
}
