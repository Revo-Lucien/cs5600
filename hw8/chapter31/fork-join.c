#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

sem_t s;

void *child(void *arg) {
    sleep(1);                 // make sure the parent really does wait
    printf("child\n");
    sem_post(&s);             // signal: child is done
    return NULL;
}

int main(int argc, char *argv[]) {
    pthread_t p;
    printf("parent: begin\n");
    sem_init(&s, 0, 0);       // initial value = 0
    pthread_create(&p, NULL, child, NULL);
    sem_wait(&s);             // block until child posts
    printf("parent: end\n");
    return 0;
}
