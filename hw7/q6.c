// Q6: hash table, single global lock vs one lock per bucket.
// gcc -O2 -pthread -o q6 q6.c && ./q6 [maxthreads]
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

#define NBUCKETS 4096
#define KEYRANGE 1000000
#define OPS      200000     // per thread, 50% insert / 50% lookup

typedef struct node { int key; struct node *next; } node_t;

typedef struct {
    node_t *head;
    pthread_mutex_t lock;   // per-bucket lock
} bucket_t;

typedef struct {
    bucket_t bucket[NBUCKETS];
    pthread_mutex_t biglock; // used only in coarse mode
} hash_t;

hash_t ht;
int coarse;                  // 1 = single lock, 0 = per-bucket locks

void hash_init(hash_t *h) {
    for (int i = 0; i < NBUCKETS; i++) {
        h->bucket[i].head = NULL;
        pthread_mutex_init(&h->bucket[i].lock, NULL);
    }
    pthread_mutex_init(&h->biglock, NULL);
}

static pthread_mutex_t *lock_for(hash_t *h, int b) {
    return coarse ? &h->biglock : &h->bucket[b].lock;
}

void hash_insert(hash_t *h, int key) {
    node_t *n = malloc(sizeof(node_t));
    n->key = key;
    int b = key % NBUCKETS;
    pthread_mutex_t *l = lock_for(h, b);
    pthread_mutex_lock(l);
    n->next = h->bucket[b].head;
    h->bucket[b].head = n;
    pthread_mutex_unlock(l);
}

int hash_lookup(hash_t *h, int key) {
    int b = key % NBUCKETS;
    pthread_mutex_t *l = lock_for(h, b);
    pthread_mutex_lock(l);
    node_t *c = h->bucket[b].head;
    while (c && c->key != key) c = c->next;
    int found = c != NULL;
    pthread_mutex_unlock(l);
    return found;
}

void hash_free(hash_t *h) {
    for (int i = 0; i < NBUCKETS; i++) {
        node_t *c = h->bucket[i].head;
        while (c) { node_t *n = c->next; free(c); c = n; }
    }
}

void *worker(void *arg) {
    unsigned seed = (unsigned)(long)arg + 1;
    for (int i = 0; i < OPS; i++) {
        int key = rand_r(&seed) % KEYRANGE;
        if (i & 1) hash_insert(&ht, key); else hash_lookup(&ht, key);
    }
    return NULL;
}

double now() {
    struct timeval t;
    gettimeofday(&t, NULL);
    return t.tv_sec + t.tv_usec / 1e6;
}

double run(int n, int mode) {
    coarse = mode;
    hash_init(&ht);
    pthread_t tid[n];
    double s = now();
    for (long i = 0; i < n; i++) pthread_create(&tid[i], NULL, worker, (void *)i);
    for (int i = 0; i < n; i++) pthread_join(tid[i], NULL);
    double t = now() - s;
    hash_free(&ht);
    return t;
}

int main(int argc, char *argv[]) {
    int maxthreads = argc > 1 ? atoi(argv[1]) : 8;
    printf("threads  single-lock(s)  per-bucket(s)\n");
    for (int n = 1; n <= maxthreads; n++) {
        double a = run(n, 1);
        double b = run(n, 0);
        printf("%7d  %14.3f  %13.3f\n", n, a, b);
    }
    return 0;
}
