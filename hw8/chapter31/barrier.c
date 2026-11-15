#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

typedef struct __barrier_t {
    int num_threads;
    int count;
    sem_t mutex;
    sem_t turnstile;
} barrier_t;

barrier_t b;

void barrier_init(barrier_t *b, int num_threads) {
    b->num_threads = num_threads;
    b->count = 0;
    sem_init(&b->mutex, 0, 1);
    sem_init(&b->turnstile, 0, 0);
}

void barrier(barrier_t *b) {
    sem_wait(&b->mutex);
    b->count++;
    if (b->count == b->num_threads)
        sem_post(&b->turnstile);   // last arrival opens the turnstile
    sem_post(&b->mutex);

    sem_wait(&b->turnstile);       // everyone waits here...
    sem_post(&b->turnstile);       // ...then lets the next thread through
}

typedef struct __tinfo_t {
    int thread_id;
} tinfo_t;

void *child(void *arg) {
    tinfo_t *t = (tinfo_t *) arg;
    printf("child %d: before\n", t->thread_id);
    barrier(&b);
    printf("child %d: after\n", t->thread_id);
    return NULL;
}

int main(int argc, char *argv[]) {
    assert(argc == 2);
    int num_threads = atoi(argv[1]);
    assert(num_threads > 0);

    pthread_t p[num_threads];
    tinfo_t t[num_threads];

    printf("parent: begin\n");
    barrier_init(&b, num_threads);

    for (int i = 0; i < num_threads; i++) {
        t[i].thread_id = i;
        pthread_create(&p[i], NULL, child, &t[i]);
    }
    for (int i = 0; i < num_threads; i++)
        pthread_join(p[i], NULL);

    printf("parent: end\n");
    return 0;
}
