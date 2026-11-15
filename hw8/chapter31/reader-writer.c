#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>

typedef struct _rwlock_t {
    sem_t lock;       // protects the readers count
    sem_t writelock;  // held by one writer, or by the readers as a group
    int readers;
} rwlock_t;

rwlock_t rw;
int shared = 0;
double t0;

double now(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec / 1e9 - t0;
}

void rwlock_init(rwlock_t *rw) {
    rw->readers = 0;
    sem_init(&rw->lock, 0, 1);
    sem_init(&rw->writelock, 0, 1);
}

void rwlock_acquire_readlock(rwlock_t *rw) {
    sem_wait(&rw->lock);
    rw->readers++;
    if (rw->readers == 1)           // first reader locks out writers
        sem_wait(&rw->writelock);
    sem_post(&rw->lock);
}

void rwlock_release_readlock(rwlock_t *rw) {
    sem_wait(&rw->lock);
    rw->readers--;
    if (rw->readers == 0)           // last reader lets writers in
        sem_post(&rw->writelock);
    sem_post(&rw->lock);
}

void rwlock_acquire_writelock(rwlock_t *rw) { sem_wait(&rw->writelock); }
void rwlock_release_writelock(rwlock_t *rw) { sem_post(&rw->writelock); }

typedef struct { int id; int delay_ms; int loops; } arg_t;

void *reader(void *arg) {
    arg_t *a = (arg_t *) arg;
    usleep(a->delay_ms * 1000);
    for (int i = 0; i < a->loops; i++) {
        rwlock_acquire_readlock(&rw);
        printf("%5.2fs reader %d: reading  shared=%d\n", now(), a->id, shared);
        sleep(1);                   // hold the lock so readers overlap
        printf("%5.2fs reader %d: done\n", now(), a->id);
        rwlock_release_readlock(&rw);
    }
    return NULL;
}

void *writer(void *arg) {
    arg_t *a = (arg_t *) arg;
    usleep(a->delay_ms * 1000);
    for (int i = 0; i < a->loops; i++) {
        printf("%5.2fs writer %d: waiting\n", now(), a->id);
        rwlock_acquire_writelock(&rw);
        shared++;
        printf("%5.2fs writer %d: WRITING  shared=%d\n", now(), a->id, shared);
        sleep(1);
        printf("%5.2fs writer %d: done\n", now(), a->id);
        rwlock_release_writelock(&rw);
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 2 || (strcmp(argv[1], "basic") && strcmp(argv[1], "starve"))) {
        fprintf(stderr, "usage: %s basic|starve\n", argv[0]);
        return 1;
    }
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    t0 = ts.tv_sec + ts.tv_nsec / 1e9;

    rwlock_init(&rw);
    pthread_t th[8];
    arg_t args[8];
    int n = 0;

    if (!strcmp(argv[1], "basic")) {
        // 3 readers and 1 writer, all starting at once
        for (int i = 0; i < 3; i++) {
            args[n] = (arg_t){ i, 0, 2 };
            pthread_create(&th[n], NULL, reader, &args[n]); n++;
        }
        args[n] = (arg_t){ 0, 0, 2 };
        pthread_create(&th[n], NULL, writer, &args[n]); n++;
    } else {
        // 6 readers start 0.5s apart, each holds the lock 1s,
        // so there is always at least one reader inside.
        // The writer arrives at 0.7s.
        for (int i = 0; i < 6; i++) {
            args[n] = (arg_t){ i, i * 500, 1 };
            pthread_create(&th[n], NULL, reader, &args[n]); n++;
        }
        args[n] = (arg_t){ 0, 700, 1 };
        pthread_create(&th[n], NULL, writer, &args[n]); n++;
    }

    for (int i = 0; i < n; i++)
        pthread_join(th[i], NULL);
    return 0;
}
