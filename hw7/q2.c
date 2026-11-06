// Q2: simple concurrent counter. gcc -O2 -pthread -o q2 q2.c && nproc && ./q2 8
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

#define INCREMENTS 1000000

typedef struct { int value; pthread_mutex_t lock; } counter_t;
counter_t c;

void *worker(void *arg) {
    for (int i = 0; i < INCREMENTS; i++) {
        pthread_mutex_lock(&c.lock);
        c.value++;
        pthread_mutex_unlock(&c.lock);
    }
    return NULL;
}

double now() {
    struct timeval t;
    gettimeofday(&t, NULL);
    return t.tv_sec + t.tv_usec / 1e6;
}

int main(int argc, char *argv[]) {
    int maxthreads = argc > 1 ? atoi(argv[1]) : 8;
    for (int n = 1; n <= maxthreads; n++) {
        pthread_t tid[n];
        c.value = 0;
        pthread_mutex_init(&c.lock, NULL);
        double start = now();
        for (int i = 0; i < n; i++) pthread_create(&tid[i], NULL, worker, NULL);
        for (int i = 0; i < n; i++) pthread_join(tid[i], NULL);
        double end = now();
        printf("threads=%d  time=%.3f s  value=%d (expected %d)\n",
               n, end - start, c.value, n * INCREMENTS);
    }
    return 0;
}
