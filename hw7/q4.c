// Q4: hand-over-hand list vs single-lock list.
// gcc -O2 -pthread -o q4 q4.c && ./q4 [maxthreads]
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <pthread.h>
#include <sys/time.h>

#define INIT_SIZE 1000
#define KEYRANGE  2000
#define OPS       5000      // per thread, 90% lookup / 10% insert

typedef struct node {
    int key;
    struct node *next;
    pthread_mutex_t lock;
} node_t;

typedef struct {
    node_t *head;            // dummy head, key = INT_MIN
    pthread_mutex_t biglock; // used only by the single-lock version
} list_t;

node_t *new_node(int key) {
    node_t *n = malloc(sizeof(node_t));
    n->key = key;
    n->next = NULL;
    pthread_mutex_init(&n->lock, NULL);
    return n;
}

void list_init(list_t *l) {
    l->head = new_node(INT_MIN);
    pthread_mutex_init(&l->biglock, NULL);
}

/* ---------- single lock, sorted list ---------- */
void sl_insert(list_t *l, int key) {
    pthread_mutex_lock(&l->biglock);
    node_t *prev = l->head, *curr = prev->next;
    while (curr && curr->key < key) { prev = curr; curr = curr->next; }
    node_t *n = new_node(key);
    n->next = curr;
    prev->next = n;
    pthread_mutex_unlock(&l->biglock);
}

int sl_lookup(list_t *l, int key) {
    pthread_mutex_lock(&l->biglock);
    node_t *curr = l->head->next;
    while (curr && curr->key < key) curr = curr->next;
    int found = curr && curr->key == key;
    pthread_mutex_unlock(&l->biglock);
    return found;
}

/* ---------- hand-over-hand (lock coupling), sorted list ---------- */
void hoh_insert(list_t *l, int key) {
    node_t *prev = l->head;
    pthread_mutex_lock(&prev->lock);
    node_t *curr = prev->next;
    if (curr) pthread_mutex_lock(&curr->lock);
    while (curr && curr->key < key) {
        pthread_mutex_unlock(&prev->lock);
        prev = curr;
        curr = curr->next;
        if (curr) pthread_mutex_lock(&curr->lock);
    }
    node_t *n = new_node(key);
    n->next = curr;
    prev->next = n;
    if (curr) pthread_mutex_unlock(&curr->lock);
    pthread_mutex_unlock(&prev->lock);
}

int hoh_lookup(list_t *l, int key) {
    node_t *prev = l->head;
    pthread_mutex_lock(&prev->lock);
    node_t *curr = prev->next;
    if (curr) pthread_mutex_lock(&curr->lock);
    while (curr && curr->key < key) {
        pthread_mutex_unlock(&prev->lock);
        prev = curr;
        curr = curr->next;
        if (curr) pthread_mutex_lock(&curr->lock);
    }
    int found = curr && curr->key == key;
    if (curr) pthread_mutex_unlock(&curr->lock);
    pthread_mutex_unlock(&prev->lock);
    return found;
}

list_t list;
int use_hoh;

void *worker(void *arg) {
    unsigned seed = (unsigned)(long)arg + 1;
    for (int i = 0; i < OPS; i++) {
        int key = rand_r(&seed) % KEYRANGE;
        if (rand_r(&seed) % 10 == 0) {
            if (use_hoh) hoh_insert(&list, key); else sl_insert(&list, key);
        } else {
            if (use_hoh) hoh_lookup(&list, key); else sl_lookup(&list, key);
        }
    }
    return NULL;
}

double now() {
    struct timeval t;
    gettimeofday(&t, NULL);
    return t.tv_sec + t.tv_usec / 1e6;
}

double run(int n, int hoh) {
    list_init(&list);
    use_hoh = hoh;
    for (int i = 0; i < INIT_SIZE; i++) sl_insert(&list, rand() % KEYRANGE);
    pthread_t tid[n];
    double s = now();
    for (long i = 0; i < n; i++) pthread_create(&tid[i], NULL, worker, (void *)i);
    for (int i = 0; i < n; i++) pthread_join(tid[i], NULL);
    return now() - s;
}

int main(int argc, char *argv[]) {
    int maxthreads = argc > 1 ? atoi(argv[1]) : 8;
    printf("threads  single-lock(s)  hand-over-hand(s)\n");
    for (int n = 1; n <= maxthreads; n++) {
        double a = run(n, 0);
        double b = run(n, 1);
        printf("%7d  %14.3f  %17.3f\n", n, a, b);
    }
    return 0;
}
