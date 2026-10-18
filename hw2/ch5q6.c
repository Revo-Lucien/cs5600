#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
	int rc = fork();
	if (rc == 0) {
		printf("child (pid %d) running\n", getpid());
	} else {
		int w = waitpid(rc, NULL, 0);
		printf("parent: waitpid() returned %d (child pid was %d)\n", w, rc);
	}
	return 0;
}
