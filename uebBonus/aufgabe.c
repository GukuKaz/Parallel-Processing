
/** gravitation simulator
  * no collision handling
  * using newton's law of universal gravitation
  *
  * mpirun -np N [-machinefile /path/to/Machinefile] aufgabe DATAFILE LOGFILE TIMESTEP ITERATIONNUM
  *
  * N - number of processes to create
  * Machinefile - machinefile that should be used (only with openmpi)
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
#include <stddef.h>
#include <mpi.h>

#define TIMESTEP        0.001
#define NUM_ITERATION    100000

#include "masspoint.h"      // zwecks Priorisierung nach den defines in main


MPI_Status status;
int my_rank, commsize;

void calc_workspace(int massnum, int *recvcounts, int *displs) {
  div_t tmpdiv = div(massnum, commsize);
  int i, rest = tmpdiv.rem;
  displs[0] = 0;
  for (i = 0; i < commsize; i++) {
    recvcounts[i] = tmpdiv.quot;
    if (rest>0) {
      recvcounts[i] += 1;
      rest--;
    }
    if (i>0) {
      displs[i] = displs[i-1] + recvcounts[i-1];
    }
  }
}

void send_masspntarr(masspoint *masspntarr, int massnum) {
  double massarr[massnum];
  double loc_xarr[massnum];
  double loc_yarr[massnum];
  double vel_xarr[massnum];
  double vel_yarr[massnum];
  int i;

  for (i = 0; i < massnum; i++) {
    massarr[i] = masspntarr[i].mass;
    loc_xarr[i] = masspntarr[i].loc[0];
    loc_yarr[i] = masspntarr[i].loc[1];
    vel_xarr[i] = masspntarr[i].vel[0];
    vel_yarr[i] = masspntarr[i].vel[1];
  }

  MPI_Bcast(massarr, massnum, MPI_DOUBLE, 0, MPI_COMM_WORLD);
  MPI_Bcast(loc_xarr, massnum, MPI_DOUBLE, 0, MPI_COMM_WORLD);
  MPI_Bcast(loc_yarr, massnum, MPI_DOUBLE, 0, MPI_COMM_WORLD);
  MPI_Bcast(vel_xarr, massnum, MPI_DOUBLE, 0, MPI_COMM_WORLD);
  MPI_Bcast(vel_yarr, massnum, MPI_DOUBLE, 0, MPI_COMM_WORLD);
}

void recv_masspntarr(masspoint *masspntarr, int massnum) {
  double massarr[massnum];
  double loc_xarr[massnum];
  double loc_yarr[massnum];
  double vel_xarr[massnum];
  double vel_yarr[massnum];
  int i;

  MPI_Bcast(massarr, massnum, MPI_DOUBLE, 0, MPI_COMM_WORLD);
  MPI_Bcast(loc_xarr, massnum, MPI_DOUBLE, 0, MPI_COMM_WORLD);
  MPI_Bcast(loc_yarr, massnum, MPI_DOUBLE, 0, MPI_COMM_WORLD);
  MPI_Bcast(vel_xarr, massnum, MPI_DOUBLE, 0, MPI_COMM_WORLD);
  MPI_Bcast(vel_yarr, massnum, MPI_DOUBLE, 0, MPI_COMM_WORLD);

  for ( i = 0; i < massnum; i++) {
    masspntarr[i].mass = massarr[i];
    masspntarr[i].loc[0] = loc_xarr[i];
    masspntarr[i].loc[1] = loc_yarr[i];
    masspntarr[i].vel[0] = vel_xarr[i];
    masspntarr[i].vel[1] = vel_yarr[i];
  }
}

void gather_results(masspoint *masspntarr, int massnum, int *recvcounts, int *displs) {
  int i, sendcount = recvcounts[my_rank];
  double *sendbuf = malloc(sizeof(sendbuf) * sendcount);
  double *recvbuf;
  if (my_rank == 0) {
    recvbuf = malloc(sizeof(recvbuf) * massnum);
  }

  // füllen und senden des sendbuffers mit x-Ortskoordinaten
  for (i = 0; i < sendcount; i++) {
    sendbuf[i] = masspntarr[i+displs[my_rank]].loc[0];
  }
  MPI_Gatherv(sendbuf, sendcount, MPI_DOUBLE, recvbuf, recvcounts, displs, MPI_DOUBLE, 0, MPI_COMM_WORLD);
  // speichern des recvbuf aus x-Ortskoordinaten in masspntarr
  if (my_rank==0) {
    for (i = 0; i < massnum; i++) {
      masspntarr[i].loc[0] = recvbuf[i];
    }
  }

  // füllen und senden des sendbuffers mit y-Ortskoordinaten
  for (i = 0; i < sendcount; i++) {
    sendbuf[i] = masspntarr[i+displs[my_rank]].loc[1];
  }
  MPI_Gatherv(sendbuf, sendcount, MPI_DOUBLE, recvbuf, recvcounts, displs, MPI_DOUBLE, 0, MPI_COMM_WORLD);
  // speichern des recvbuf aus y-Ortskoordinaten in masspntarr
  if (my_rank==0) {
    for (i = 0; i < massnum; i++) {
      masspntarr[i].loc[1] = recvbuf[i];
    }
  }

  // füllen und senden des sendbuffers mit x-Geschwindigkeitskoordinaten
  for (i = 0; i < sendcount; i++) {
    sendbuf[i] = masspntarr[i+displs[my_rank]].vel[0];
  }
  MPI_Gatherv(sendbuf, sendcount, MPI_DOUBLE, recvbuf, recvcounts, displs, MPI_DOUBLE, 0, MPI_COMM_WORLD);
  // speichern des recvbuf aus x-Geschwindigkeitskoordinaten in masspntarr
  if (my_rank==0) {
    for (i = 0; i < massnum; i++) {
      masspntarr[i].vel[0] = recvbuf[i];
    }
  }

  // füllen und senden des sendbuffers mit y-Ortskoordinaten
  for (i = 0; i < sendcount; i++) {
    sendbuf[i] = masspntarr[i+displs[my_rank]].vel[1];
  }
  MPI_Gatherv(sendbuf, sendcount, MPI_DOUBLE, recvbuf, recvcounts, displs, MPI_DOUBLE, 0, MPI_COMM_WORLD);
  // speichern des recvbuf aus y-Ortskoordinaten in masspntarr
  if (my_rank==0) {
    for (i = 0; i < massnum; i++) {
      masspntarr[i].vel[1] = recvbuf[i];
    }
  }
}

int main(int argc, char *argv[]) {
  time_t timestart = time(NULL);
  int massnum = 0;
  double stepsize = 0;

  // für MPI
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &commsize);

  if (my_rank == 0) {
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
    int i=0;
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

    // Senden von massnum
    MPI_Bcast(&massnum, 1, MPI_INT, 0, MPI_COMM_WORLD);
    // Senden von stepsize
    MPI_Bcast(&stepsize, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // Berechnung des Arbeitsbereichs
    int recvcounts[commsize];
    int displs[commsize];
    calc_workspace(massnum, recvcounts, displs);

    //open logfile
    FILE* logfile = fopen(logfname, "w");

    const char* formatstr = "mass: % 18.6lf | x: % 18.6lf | y: % 18.6lf | vel_x: % 18.6lf | vel_y: % 18.6lf\n";
    printf("Initialized masspoints\n");
    fprintf(logfile, "Initialized masspoints\n");
    for (i = 0; i < massnum; i++) {
      printf(          formatstr, masspntarr[i].mass, masspntarr[i].loc[0], masspntarr[i].loc[1], masspntarr[i].vel[0], masspntarr[i].vel[1]);
      fprintf(logfile, formatstr, masspntarr[i].mass, masspntarr[i].loc[0], masspntarr[i].loc[1], masspntarr[i].vel[0], masspntarr[i].vel[1]);
    }

    formatstr = "x: % 18.6lf | y: % 18.6lf | vel_x: % 18.6lf | vel_y: % 18.6lf\n";

    char marker = 1; // Marker für Schleife bei Workerprozessen
    long a;
    for (a = 0; a < iterationnum; a++) {
      printf("\n\n");
      fprintf(logfile, "\n\n");

      // Senden des Markers
      MPI_Bcast(&marker, 1, MPI_CHAR, 0, MPI_COMM_WORLD);

      // Berechnung der Bewegung des zugewiesenen Arbeitsbereichs
      send_masspntarr(masspntarr, massnum);
      calc_new_vel(masspntarr, massnum, stepsize, displs[my_rank], displs[my_rank] + recvcounts[my_rank]);
      move(masspntarr, stepsize, displs[my_rank], displs[my_rank] + recvcounts[my_rank]);

      // Verteilen der Ergebnisse
      gather_results(masspntarr, massnum, recvcounts, displs);
      simtime += stepsize;

      printf("After %18.6g seconds\n", simtime);
      fprintf(logfile, "After %18.6g seconds\n", simtime);
      for (i = 0; i < massnum; i++) {
        printf(formatstr, masspntarr[i].loc[0], masspntarr[i].loc[1], masspntarr[i].vel[0], masspntarr[i].vel[1]);
        fprintf(logfile, formatstr, masspntarr[i].loc[0], masspntarr[i].loc[1], masspntarr[i].vel[0], masspntarr[i].vel[1]);
      }
    }
    marker = 0;
    MPI_Bcast(&marker, 1, MPI_CHAR, 0, MPI_COMM_WORLD);

    time_t finishtime = time(NULL) - timestart;
    printf("Finished after: %12ld sec\n", finishtime);
    fprintf(logfile, "Finished after: %12ld sec\n", finishtime);

    fclose(logfile);
  } else {
    int source = 0;
    // Empfangen von massnum
    MPI_Bcast(&massnum, 1, MPI_INT, source, MPI_COMM_WORLD);
    // Empfangen von stepsize
    MPI_Bcast(&stepsize, 1, MPI_DOUBLE, source, MPI_COMM_WORLD);
    // Berechnung des Arbeitsbereichs
    int recvcounts[commsize];
    int displs[commsize];
    calc_workspace(massnum, recvcounts, displs);

    masspoint masspntarr[massnum];

    // Empfangen des Markers
    char marker = 0;
    MPI_Bcast(&marker, 1, MPI_CHAR, source, MPI_COMM_WORLD);
    do {
      // Berechnung der Bewegung des zugewiesenen Arbeitsbereichs
      recv_masspntarr(masspntarr, massnum);
      calc_new_vel(masspntarr, massnum, stepsize, displs[my_rank], displs[my_rank] + recvcounts[my_rank]);
      move(masspntarr, stepsize, displs[my_rank], displs[my_rank] + recvcounts[my_rank]);

      // Verteilen der Ergebnisse
      gather_results(masspntarr, massnum, recvcounts, displs);
      // Empfangen des Markers
      MPI_Bcast(&marker, 1, MPI_CHAR, source, MPI_COMM_WORLD);
    } while(marker == 1);
  }

  MPI_Finalize();
  return 0;
}
