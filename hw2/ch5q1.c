#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
	int x= 0;
	int rc = fork();
	if (rc == 0) {
		x = 1;
		printf("Child is running. x is equal to %d \n", x);
	} else {
		x = 2;
		printf("Parent is running. x is equal to %d \n", x);
	}
	return 0;
}
