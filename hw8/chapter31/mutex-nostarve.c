#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define NUM_THREADS 4
#define LOOPS 3

sem_t guard;                 // protects next_ticket
sem_t slot[NUM_THREADS];     // slot[t % N] is posted when ticket t may enter
int next_ticket = 0;         // next ticket to hand out
int now_serving = 0;         // ticket currently holding the mutex

void mutex_init(void) {
    sem_init(&guard, 0, 1);
    for (int i = 0; i < NUM_THREADS; i++)
        sem_init(&slot[i], 0, i == 0 ? 1 : 0);   // ticket 0 may enter at once
}

void mutex_acquire(void) {
    sem_wait(&guard);
    int my = next_ticket++;                      // take a ticket
    sem_post(&guard);
    sem_wait(&slot[my % NUM_THREADS]);           // wait for my turn
}

void mutex_release(void) {
    now_serving++;                               // only the holder touches this
    sem_post(&slot[now_serving % NUM_THREADS]);  // wake the next ticket
}

void *worker(void *arg) {
    long id = (long) arg;
    for (int i = 0; i < LOOPS; i++) {
        mutex_acquire();
        printf("thread %ld: in critical section\n", id);
        usleep(100000);                          // hold it briefly
        mutex_release();
    }
    return NULL;
}

int main(void) {
    pthread_t th[NUM_THREADS];
    mutex_init();
    for (long i = 0; i < NUM_THREADS; i++)
        pthread_create(&th[i], NULL, worker, (void *) i);
    for (int i = 0; i < NUM_THREADS; i++)
        pthread_join(th[i], NULL);
    printf("done\n");
    return 0;
}
