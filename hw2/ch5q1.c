#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
	int x = 100;
	int rc = fork();
	if (rc == 0) {
		printf("Child: x starts as %d\n", x);   // value right after fork
		x = 1;
		printf("Child: x is now %d\n", x);
	} else {
		printf("Parent: x starts as %d\n", x);  // value right after fork
		x = 2;
		printf("Parent: x is now %d\n", x);
		wait(NULL);                              // let the child finish first
		printf("Parent: x is still %d after child exits\n", x);
	}
	return 0;
}
