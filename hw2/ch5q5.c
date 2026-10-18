#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
	int rc = fork();
	if (rc == 0) {
		int w = wait(NULL);
		printf("child (pid %d): wait() returned %d\n", getpid(), w);
	} else {
		int w = wait(NULL);
		printf("parent: wait() returned %d (child pid was %d)\n", w, rc);
	}
	return 0;
}
