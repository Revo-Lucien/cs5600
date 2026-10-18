#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>

int main() {
	struct timeval a, b;

	// timer precision: back-to-back gettimeofday calls
	gettimeofday(&a, NULL);
	gettimeofday(&b, NULL);
	printf("back-to-back gettimeofday: %ld us apart\n",
	       (b.tv_sec - a.tv_sec) * 1000000L + (b.tv_usec - a.tv_usec));

	// system call cost: many 0-byte reads
	int n = 1000000;
	char c;
	gettimeofday(&a, NULL);
	for (int i = 0; i < n; i++)
		read(0, &c, 0);
	gettimeofday(&b, NULL);

	double us = (b.tv_sec - a.tv_sec) * 1000000.0 + (b.tv_usec - a.tv_usec);
	printf("total: %.0f us, per syscall: %.4f us (%.1f ns)\n",
	       us, us / n, us / n * 1000);
	return 0;
}
