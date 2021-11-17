#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/wait.h>

int main()
{
    // fd[0] - read
    // fd[1] - write
    int fd[2];
    if (pipe(fd) == -1) {
        printf("Failed opening the pipe\n");
        exit(1);
    }


    pid_t pid = fork();
    if (pid == -1) {
        printf("Fork failed\n");
        exit(2);
    }

    long values[2];
    if (pid == 0) {
        close(fd[1]);
        sleep(3);
        printf("Getting child time...\n");
        if (read(fd[0], values, sizeof(long) * 2) == -1) {
            printf("Failed data reading from the pipe\n");
            exit(4);
        }

        time_t timer = time(0);
        printf("Child's time: %s", ctime(&timer));
        printf("Got from parent process:\n * time - %s * pid - %ld\n", ctime(&values[0]), values[1]);
        close(fd[0]);
    } else {
        close(fd[0]);
        printf("Getting parent time and the pid...\n");
        values[0] = time(0);
        values[1] = getpid();

        sleep(5);
        if (write(fd[1], values , sizeof(long) * 2) == -1) {
            printf("Failed data writing in to the pipe\n");
            exit(3);
        }
        close(fd[1]);
        wait(NULL);
    }

    return 0;
}
