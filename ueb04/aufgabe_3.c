#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>

int philosophers;

typedef struct {
  pthread_t thread;
  int id;
  pthread_mutex_t* first;
  pthread_mutex_t* second;
} philo_attr_t;

void* philosoph(void* arg) {
  philo_attr_t* self = arg;
  enum {
    Thinking,
    Eating
  } state = Thinking;

  while (1) {
    if (state == Thinking) {
      pthread_mutex_lock(self->first);
      fprintf(stderr, "Philosoph %i: Got first fork (%i)\n", self->id, self->id);

      while (pthread_mutex_trylock(self->second) != 0) {
        pthread_mutex_unlock(self->first);
        fprintf(stderr, "Philosoph %i: Gave up first fork (%i)\n", self->id, self->id);
        pthread_mutex_lock(self->first);
        fprintf(stderr, "Philosoph %i: Got first fork (%i)\n", self->id, self->id);
      }

      fprintf(stderr, "Philosoph %i: Got second fork (%i)\n", self->id, (self->id+1) % philosophers);
      fprintf(stderr, "Philosoph %i: Eating\n", self->id);
      state = Eating;
    } else {
      pthread_mutex_unlock(self->second);
      fprintf(stderr, "Philosoph %i: Returned second fork (%i)\n", self->id, (self->id+1) % philosophers);
      pthread_mutex_unlock(self->first);
      fprintf(stderr, "Philosoph %i: Returned first fork (%i)\n", self->id, self->id);
      fprintf(stderr, "Philosoph %i: Thinking\n", self->id);
      state = Thinking;
    }

    // sorgt für Fairness zwischen den Philosophen, durch freiwillige Abgabe an den OS-Scheduler
    sched_yield();
  }
}

int main(int argc, char const *argv[]) {
  if (argc>1) {
    philosophers = atoi(argv[1]);
  } else {
    philosophers = 3;
  }

  pthread_mutex_t forks[philosophers];
  philo_attr_t philos[philosophers];

  for (int i = 0; i < philosophers; i++) {
    pthread_mutex_init(&forks[i], NULL);
  }

  for (int i = 0; i < philosophers; i++) {
    philos[i].id = i;
    philos[i].first = &forks[i];
    philos[i].second = &forks[(i+1) % philosophers];

    // zyklische Abhängigkeit durch Priorisierung der Gabeln gebrochen
    /*
    if (philos[i].first > philos[i].second) {
      pthread_mutex_t* tmp = philos[i].first;
      philos[i].first = philos[i].second;
      philos[i].second = tmp;
    }
    */

    pthread_create(&philos[i].thread, NULL, philosoph, &philos[i]);
  }

  sleep(1);
}
