
/** gravitation simulator
  * no collision handling
  * using newton's law of universal gravitation
  *
  * aufgabe1 DATAFILE LOGFILE TIMESTEP ITERATIONNUM
  *
  * DATAFILE - file with the data for the masspoints in the format of:
  *            mass,loc_x,loc_y,vel_x,vel_y
  *            the readed numbers are rounded to 6th decimal place
  *            the readed numbers are correct if they are less than 8589934592.000000
  * LOGFILE  - path to file (max 32 chars long) where the simulation information will be saved
  * taskset -c <liste von Prozessoren die genutzt werden soll> <Programm was ausgeführt werden soll>
  *   für antackern auf einen Prozessor
  */

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

#define TIMESTEP        0.001
#define NUM_ITERATION    100000

#include "masspoint2.h"      // zwecks Priorisierung nach den defines in main


int main(int argc, char const *argv[]) {
  time_t start = time(NULL);
  long massnum = 0;
  double stepsize = 0;
  long iterationnum = 0;
  char *logfname = malloc(32 * sizeof(logfname));
  char *datafname = malloc(32 * sizeof(datafname));
  double simtime = 0;

  time_t t;
  struct tm *tstruc;

  // BEGIN auslesen
  // datafile
  if (argc == 1) {
    datafname = "testfile.csv";
  } else {
    datafname = (char *) argv[1];
  }

  // logfile
  if (argc <= 2) {
    t = time(NULL);
    tstruc = localtime(&t);
    sprintf(logfname, "bin/log-%d-%d-%d-%d-%d.txt",
    tstruc->tm_year+1900, tstruc->tm_mon+1, tstruc->tm_mday, tstruc->tm_hour,
    tstruc->tm_min);
  } else {
    logfname = (char *) argv[2];
  }

  // stepsize
  if (argc <= 3) {
    stepsize = TIMESTEP;
  } else {
    stepsize = atof(argv[3]);
  }

  // iterationnum
  if (argc <= 4) {
    iterationnum = NUM_ITERATION;
  } else {
    iterationnum = atol(argv[4]);
  }
  // END auslesen

  //define the input
  // count lines
  double tmp[5];
  FILE* datafile = fopen(datafname, "r");
  char* tmpstr = malloc(1024 * sizeof(tmpstr));
  while (fgets(tmpstr, 1024, datafile)) {
    massnum++;
  }
  fclose(datafile);

  masspoint masspntarr[massnum];

  // csv_to_masspntarr
  long i=0;
  datafile = fopen(datafname, "r");
  while (fgets(tmpstr, 1024, datafile)) {
    sscanf(tmpstr, "%lf,%lf,%lf,%lf,%lf,\n", &tmp[0], &tmp[1], &tmp[2], &tmp[3], &tmp[4]);

    masspntarr[i] = (masspoint) {
      .mass = tmp[0],
      .loc = { tmp[1], tmp[2] },
      .vel = { tmp[3], tmp[4] }
    };
    i++;
  }
  fclose(datafile);
  free(tmpstr);

  //open logfile
  FILE* logfile = fopen(logfname, "w");

  const char* formatstr = "mass: % 18.6lf | x: % 18.6lf | y: % 18.6lf | vel_x: % 18.6lf | vel_y: %0 18.6lf\n";
  printf("Initialized masspoints\n");
  fprintf(logfile, "Initialized masspoints\n");
  for (int i = 0; i < massnum; i++) {
    printf(          formatstr, masspntarr[i].mass, masspntarr[i].loc[0], masspntarr[i].loc[1], masspntarr[i].vel[0], masspntarr[i].vel[1]);
    fprintf(logfile, formatstr, masspntarr[i].mass, masspntarr[i].loc[0], masspntarr[i].loc[1], masspntarr[i].vel[0], masspntarr[i].vel[1]);
  }

  formatstr = "x: % 18.6lf | y: % 18.6lf | vel_x: % 18.6lf | vel_y: % 18.6lf\n";

  for (int i = 0; i < iterationnum; i++) {
    printf("\n\n");
    fprintf(logfile, "\n\n");

    move(masspntarr, massnum, stepsize);
    simtime += stepsize;

    printf("After %18.6g seconds\n", simtime);
    fprintf(logfile, "After %18.6g seconds\n", simtime);
    for (int i = 0; i < massnum; i++) {
      printf(formatstr, masspntarr[i].loc[0], masspntarr[i].loc[1], masspntarr[i].vel[0], masspntarr[i].vel[1]);
      fprintf(logfile, formatstr, masspntarr[i].loc[0], masspntarr[i].loc[1], masspntarr[i].vel[0], masspntarr[i].vel[1]);
    }
  }

  time_t finishtime = time(NULL) - start;
  printf("Finished after: %12ld sec\n", finishtime);
  fprintf(logfile, "Finished after: %12ld sec\n", finishtime);

  fclose(logfile);

  return 0;
}
