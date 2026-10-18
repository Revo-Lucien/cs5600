#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int main() {
	int fd = open("ch5q2.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
	int rc = fork();
	if (rc == 0) {
		write(fd, "child\n", 6);
	} else {
		write(fd, "parent\n", 7);
	}
	return 0;
}
