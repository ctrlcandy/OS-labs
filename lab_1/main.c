#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

void show_pid_ppid() {
    printf("atexit() call. Current ID: %d, parent ID: %d\n", getpid(), getppid());
}

int main() {
    atexit(show_pid_ppid);

    pid_t pid = fork();
    switch (pid) {
        case -1: {
            exit(1);
        }
        case 0: {
            printf("Child process\n");
            exit(0);
        }
        default: {
            int status;
            printf("Parent process. Waiting for a child process to finish.\n");
            waitpid(pid, &status, 0);
            printf("Status of the child process: %d\n", WEXITSTATUS(status));
        }
    }
    return 0;
}
