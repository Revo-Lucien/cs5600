// Q5: hash table with a single global lock.
// gcc -O2 -pthread -o q5 q5.c && ./q5 [maxthreads]
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

#define NBUCKETS 4096
#define KEYRANGE 1000000
#define OPS      200000     // per thread, 50% insert / 50% lookup

typedef struct node { int key; struct node *next; } node_t;

typedef struct {
    node_t *bucket[NBUCKETS];
    pthread_mutex_t lock;
} hash_t;

hash_t ht;

void hash_init(hash_t *h) {
    for (int i = 0; i < NBUCKETS; i++) h->bucket[i] = NULL;
    pthread_mutex_init(&h->lock, NULL);
}

void hash_insert(hash_t *h, int key) {
    node_t *n = malloc(sizeof(node_t));
    n->key = key;
    pthread_mutex_lock(&h->lock);
    int b = key % NBUCKETS;
    n->next = h->bucket[b];
    h->bucket[b] = n;
    pthread_mutex_unlock(&h->lock);
}

int hash_lookup(hash_t *h, int key) {
    pthread_mutex_lock(&h->lock);
    node_t *c = h->bucket[key % NBUCKETS];
    while (c && c->key != key) c = c->next;
    int found = c != NULL;
    pthread_mutex_unlock(&h->lock);
    return found;
}

void hash_free(hash_t *h) {
    for (int i = 0; i < NBUCKETS; i++) {
        node_t *c = h->bucket[i];
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

int main(int argc, char *argv[]) {
    int maxthreads = argc > 1 ? atoi(argv[1]) : 8;
    for (int n = 1; n <= maxthreads; n++) {
        hash_init(&ht);
        pthread_t tid[n];
        double s = now();
        for (long i = 0; i < n; i++) pthread_create(&tid[i], NULL, worker, (void *)i);
        for (int i = 0; i < n; i++) pthread_join(tid[i], NULL);
        printf("threads=%d  time=%.3f s\n", n, now() - s);
        hash_free(&ht);
    }
    return 0;
}
