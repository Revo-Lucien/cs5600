#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <sched.h>
#include <sys/time.h>
#include <sys/wait.h>

int main() {
	int p1[2], p2[2];       // p1: parent -> child, p2: child -> parent
	pipe(p1);
	pipe(p2);

	// pin this process (and the child, which inherits it) to CPU 0
	cpu_set_t set;
	CPU_ZERO(&set);
	CPU_SET(0, &set);
	sched_setaffinity(0, sizeof(set), &set);

	int n = 100000;
	char c = 'x';

	if (fork() == 0) {
		for (int i = 0; i < n; i++) {
			read(p1[0], &c, 1);
			write(p2[1], &c, 1);
		}
		return 0;
	}

	struct timeval a, b;
	gettimeofday(&a, NULL);
	for (int i = 0; i < n; i++) {
		write(p1[1], &c, 1);
		read(p2[0], &c, 1);
	}
	gettimeofday(&b, NULL);
	wait(NULL);

	double us = (b.tv_sec - a.tv_sec) * 1000000.0 + (b.tv_usec - a.tv_usec);
	printf("total: %.0f us\n", us);
	printf("per round trip: %.3f us\n", us / n);
	printf("per context switch (round trip / 2): %.3f us\n", us / n / 2);
	return 0;
}
