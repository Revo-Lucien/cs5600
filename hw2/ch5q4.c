#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
	char *args[] = {"ls", "-l", NULL};
	char *envp[] = {NULL};

	for (int i = 0; i < 6; i++) {
		if (fork() == 0) {
			printf("--- variant %d ---\n", i + 1);
			fflush(stdout);
			switch (i) {
			case 0: execl("/bin/ls", "ls", "-l", (char *)NULL); break;
			case 1: execle("/bin/ls", "ls", "-l", (char *)NULL, envp); break;
			case 2: execlp("ls", "ls", "-l", (char *)NULL); break;
			case 3: execv("/bin/ls", args); break;
			case 4: execvp("ls", args); break;
			case 5: execvpe("ls", args, envp); break;
			}
			perror("exec failed");   // only reached if exec fails
			return 1;
		}
		wait(NULL);
	}
	return 0;
}
