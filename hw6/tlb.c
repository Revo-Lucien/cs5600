#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sched.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "usage: tlb <numpages> <trials>\n");
        exit(1);
    }
    long numpages = atol(argv[1]);
    long trials   = atol(argv[2]);
    long pagesize = sysconf(_SC_PAGESIZE);
    long jump     = pagesize / sizeof(int);

    // pin to one CPU so migrations don't pollute the TLB measurement
    cpu_set_t set;
    CPU_ZERO(&set);
    CPU_SET(0, &set);
    sched_setaffinity(0, sizeof(set), &set);

    int *a = calloc(numpages, pagesize);   // calloc touches/zeros pages
    if (a == NULL) { perror("calloc"); exit(1); }

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    for (long t = 0; t < trials; t++)
        for (long i = 0; i < numpages * jump; i += jump)
            a[i] += 1;
    clock_gettime(CLOCK_MONOTONIC, &end);

    double ns = (end.tv_sec - start.tv_sec) * 1e9 + (end.tv_nsec - start.tv_nsec);
    printf("%ld pages: %.2f ns/access\n", numpages, ns / (trials * numpages));
    return 0;
}
