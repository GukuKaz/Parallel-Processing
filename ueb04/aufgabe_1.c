#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>

int philosophers;

typedef struct {
  pthread_t thread;
  int id;
  pthread_mutex_t* left;
  pthread_mutex_t* right;
} philo_attr_t;

/* Initialisierung der Philosophen */
void* philosoph(void* arg) {
  philo_attr_t* self = arg;
  enum {
    Thinking,
    Eating
  } state = Thinking;


  while (1) {
    if (state == Thinking) {
      pthread_mutex_lock(self->left);
      fprintf(stderr, "Philosoph %i: Got left fork (%i)\n", self->id, self->id);
      pthread_mutex_lock(self->right);
      fprintf(stderr, "Philosoph %i: Got right fork (%i)\n", self->id, (self->id+1) % philosophers);
      fprintf(stderr, "Philosoph %i: Eating\n", self->id);
      state = Eating;
    }
    else {
      pthread_mutex_unlock(self->right);
      fprintf(stderr, "Philosoph %i: Returned right fork (%i)\n", self->id, (self->id+1) % philosophers);
      pthread_mutex_unlock(self->left);
      fprintf(stderr, "Philosoph %i: Returned left fork (%i)\n", self->id, self->id);
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
    philos[i].left = &forks[i];
    philos[i].right = &forks[(i+1) % philosophers];
    pthread_create(&philos[i].thread, NULL, philosoph, &philos[i]);
  }

  sleep(5);
}
