// Q3: approximate counter. gcc -O2 -pthread -o q3 q3.c && ./q3 [maxthreads]
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>

#define INCREMENTS 1000000
#define MAXCPUS 128

typedef struct {
    int global;
    pthread_mutex_t glock;
    int local[MAXCPUS];
    pthread_mutex_t llock[MAXCPUS];
    int threshold;
    int numcpus;
} counter_t;

counter_t c;

void init(counter_t *c, int threshold, int numcpus) {
    c->threshold = threshold;
    c->numcpus = numcpus;
    c->global = 0;
    pthread_mutex_init(&c->glock, NULL);
    for (int i = 0; i < numcpus; i++) {
        c->local[i] = 0;
        pthread_mutex_init(&c->llock[i], NULL);
    }
}

void update(counter_t *c, int tid, int amt) {
    int cpu = tid % c->numcpus;
    pthread_mutex_lock(&c->llock[cpu]);
    c->local[cpu] += amt;
    if (c->local[cpu] >= c->threshold) {
        pthread_mutex_lock(&c->glock);
        c->global += c->local[cpu];
        pthread_mutex_unlock(&c->glock);
        c->local[cpu] = 0;
    }
    pthread_mutex_unlock(&c->llock[cpu]);
}

int get(counter_t *c) {
    pthread_mutex_lock(&c->glock);
    int v = c->global;
    pthread_mutex_unlock(&c->glock);
    return v;
}

void *worker(void *arg) {
    int tid = (int)(long)arg;
    for (int i = 0; i < INCREMENTS; i++) update(&c, tid, 1);
    return NULL;
}

double now() {
    struct timeval t;
    gettimeofday(&t, NULL);
    return t.tv_sec + t.tv_usec / 1e6;
}

int main(int argc, char *argv[]) {
    int maxthreads = argc > 1 ? atoi(argv[1]) : 8;
    int ncpu = sysconf(_SC_NPROCESSORS_ONLN);
    if (ncpu > MAXCPUS) ncpu = MAXCPUS;
    printf("cpus=%d\n", ncpu);

    // Part A: fixed threshold (1024), vary threads
    printf("\n-- vary threads, threshold=1024 --\n");
    for (int n = 1; n <= maxthreads; n++) {
        pthread_t tid[n];
        init(&c, 1024, ncpu);
        double s = now();
        for (long i = 0; i < n; i++) pthread_create(&tid[i], NULL, worker, (void *)i);
        for (int i = 0; i < n; i++) pthread_join(tid[i], NULL);
        double e = now();
        printf("threads=%d time=%.3f s global=%d\n", n, e - s, get(&c));
    }

    // Part B: fixed threads (= ncpu), vary threshold
    printf("\n-- vary threshold, threads=%d --\n", ncpu);
    for (int th = 1; th <= 65536; th *= 4) {
        pthread_t tid[ncpu];
        init(&c, th, ncpu);
        double s = now();
        for (long i = 0; i < ncpu; i++) pthread_create(&tid[i], NULL, worker, (void *)i);
        for (int i = 0; i < ncpu; i++) pthread_join(tid[i], NULL);
        double e = now();
        printf("threshold=%d time=%.3f s global=%d (expected %d)\n",
               th, e - s, get(&c), ncpu * INCREMENTS);
    }
    return 0;
}
