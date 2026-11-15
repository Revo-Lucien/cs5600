#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

sem_t lock;       // protects readers
sem_t writelock;  // held by one writer, or by the readers as a group
sem_t turnstile;  // blocks new readers while a writer is waiting
int readers = 0;

void read_lock(void) {
    sem_wait(&turnstile);        // blocks here if a writer is waiting
    sem_post(&turnstile);

    sem_wait(&lock);
    if (++readers == 1)
        sem_wait(&writelock);    // first reader locks out writers
    sem_post(&lock);
}

void read_unlock(void) {
    sem_wait(&lock);
    if (--readers == 0)
        sem_post(&writelock);    // last reader lets writers in
    sem_post(&lock);
}

void write_lock(void) {
    sem_wait(&turnstile);        // lock out new readers
    sem_wait(&writelock);        // wait for current readers to drain
    sem_post(&turnstile);
}

void write_unlock(void) {
    sem_post(&writelock);
}

void *reader(void *arg) {
    long id = (long) arg;
    usleep(id * 500000);         // reader i arrives at i * 0.5s
    printf("reader %ld: waiting\n", id);
    read_lock();
    printf("reader %ld: reading\n", id);
    sleep(1);
    printf("reader %ld: done\n", id);
    read_unlock();
    return NULL;
}

void *writer(void *arg) {
    usleep(700000);              // arrives at 0.7s
    printf("writer: waiting\n");
    write_lock();
    printf("writer: WRITING\n");
    sleep(1);
    printf("writer: done\n");
    write_unlock();
    return NULL;
}

int main(void) {
    pthread_t r[6], w;
    sem_init(&lock, 0, 1);
    sem_init(&writelock, 0, 1);
    sem_init(&turnstile, 0, 1);

    for (long i = 0; i < 6; i++)
        pthread_create(&r[i], NULL, reader, (void *) i);
    pthread_create(&w, NULL, writer, NULL);

    for (int i = 0; i < 6; i++)
        pthread_join(r[i], NULL);
    pthread_join(w, NULL);
    return 0;
}
