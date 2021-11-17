#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <errno.h>
#include <asm/errno.h>
#include <fcntl.h>

int main()
{
    if ((mkfifo("FIFO", 0777)) == -1) {
        if (errno != EEXIST) {
            printf("Couldn't create FIFO file\n");
            exit(1);
        }
    }

    pid_t pid = fork();
    if (pid == 0) {
        sleep(1);
        printf("Opening the FIFO in child process...\n");
        int fd = open("FIFO", O_RDONLY);
        if (fd == -1) {
            printf("Couldn't open FIFO file in child process\n");
            exit(2);
        }
        sleep(1);
        printf("Opened in child!\n");

        char buff[100];
        if (read(fd, buff, 100) == -1) {
            printf("Couldn't read FIFO file in child process\n");
            exit(3);
        }
        time_t timer = time(0);
        printf("Child's time: %s", ctime(&timer));
        printf("Got from parent process:\n%s", buff);

        close(fd);
    } else {
        printf("Opening the FIFO in parent process...\n");
        int fd = open("FIFO", O_WRONLY);
        if (fd == -1) {
            printf("Couldn't open FIFO file in parent process\n");
            exit(4);
        }
        printf("Opened in parent!\n");

        time_t timer = time(0);
        sleep(5);
        char buff[100];
        sprintf(buff, "* time: %s* pid: %d\n", ctime(&timer) , getpid());
        if (write(fd, buff, 100) == -1) {
            printf("Couldn't write in FIFO file in parent process\n");
            exit(5);
        }

        close(fd);
        wait(NULL);
    }

    return 0;
}
