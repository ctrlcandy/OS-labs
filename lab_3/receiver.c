#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <errno.h>

#define MEM_FILE "shared_memory"

typedef struct data {
    time_t time;
    pid_t pid;
} data;

int main() {
    // CHECKING ERRORS BLOCK
    key_t key = ftok(MEM_FILE, 1);
    if (key == -1) {
        printf("Key creation error in receiver process: %s\n", strerror(errno));
        exit(-1);
    }

    int shid = shmget(key, sizeof(time_t) + sizeof(int), 0666);
    if (shid < 0) {
        printf("Creation error in receiver process: %s\n", strerror(errno));
        exit(-1);
    }

    void *shptr = shmat(shid, NULL, 0);
    if (shptr == (void *) -1) {
        printf("Attach error in receiver process: %s\n", strerror(errno));
        exit(-1);
    }
    // ---------

    printf("* Receiving info...\n");
    data got_info = *(data *) shptr;
    sleep(5);

    time_t tm = time(NULL);
    int id = getpid();
    printf("* Receiver time: %s* Receiver pid: %d\n", ctime(&tm), id);
    sleep(1);

    printf("* Information was received: \n");
    printf("Sender time: %sSender pid: %d\n", ctime(&got_info.time), got_info.pid);

    shmdt(shptr);
    return 0;
}
