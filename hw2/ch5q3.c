#include <stdio.h>
#include <unistd.h>

int main() {
	int fds[2];
	pipe(fds);
	if (fork() == 0) {
		printf("hello\n");
		fflush(stdout);
		write(fds[1], "x", 1);
	} else {
		char c;
		read(fds[0], &c, 1);
		printf("goodbye\n");
	}
	return 0;
}
