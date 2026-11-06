// Q1: timer accuracy. gcc -O2 -o q1 q1.c && ./q1
#include <stdio.h>
#include <sys/time.h>
#include <stdint.h>
#include <x86intrin.h>

int main() {
    struct timeval a, b;
    long min_us = 1000000;
    for (int i = 0; i < 1000000; i++) {
        gettimeofday(&a, NULL);
        gettimeofday(&b, NULL);
        long d = (b.tv_sec - a.tv_sec) * 1000000L + (b.tv_usec - a.tv_usec);
        if (d > 0 && d < min_us) min_us = d;
    }
    printf("gettimeofday smallest nonzero interval: %ld us\n", min_us);

    gettimeofday(&a, NULL);
    for (int i = 0; i < 1000000; i++) gettimeofday(&b, NULL);
    long total = (b.tv_sec - a.tv_sec) * 1000000L + (b.tv_usec - a.tv_usec);
    printf("avg cost per gettimeofday call: %.3f us\n", total / 1e6);

    uint64_t t0 = __rdtsc();
    uint64_t t1 = __rdtsc();
    printf("rdtsc back-to-back delta: %lu cycles\n", (unsigned long)(t1 - t0));

    // calibrate rdtsc against gettimeofday over ~100 ms
    gettimeofday(&a, NULL);
    t0 = __rdtsc();
    do { gettimeofday(&b, NULL); }
    while ((b.tv_sec - a.tv_sec) * 1000000L + (b.tv_usec - a.tv_usec) < 100000);
    t1 = __rdtsc();
    long us = (b.tv_sec - a.tv_sec) * 1000000L + (b.tv_usec - a.tv_usec);
    printf("rdtsc rate: %.1f MHz (so 1 cycle ~ %.3f ns)\n",
           (double)(t1 - t0) / us, 1000.0 * us / (t1 - t0));
    return 0;
}
