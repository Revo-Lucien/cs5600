#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
	int rc = fork();
	if (rc == 0) {
		close(STDOUT_FILENO);
		printf("hello from the child\n");
	} else {
		wait(NULL);
		printf("parent: child is done\n");
	}
	return 0;
}
