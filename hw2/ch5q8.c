#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
	int fds[2];
	pipe(fds);                         // fds[0] = read end, fds[1] = write end

	if (fork() == 0) {
		// child 1: writer
		dup2(fds[1], STDOUT_FILENO);   // stdout now goes into the pipe
		close(fds[0]);
		close(fds[1]);
		execlp("echo", "echo", "hello through the pipe", (char *)NULL);
	}

	if (fork() == 0) {
		// child 2: reader
		dup2(fds[0], STDIN_FILENO);    // stdin now comes from the pipe
		close(fds[0]);
		close(fds[1]);
		execlp("cat", "cat", (char *)NULL);
	}

	// parent: close both ends so the reader sees end-of-file
	close(fds[0]);
	close(fds[1]);
	wait(NULL);
	wait(NULL);
	return 0;
}
