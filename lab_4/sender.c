#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <errno.h>

#define MEM_FILE "shared_memory"

struct sembuf sem_lock = {0, -1, 0}, sem_open = {0, 1, 0};

typedef struct data {
    time_t time;
    pid_t pid;
} data;

int main() {
    // CHECKING ERRORS BLOCK
    key_t key = ftok(MEM_FILE, 1);
    if (key == -1) {
        printf("Key creation error in sender process: %s\n", strerror(errno));
        exit(-1);
    }

    int shid = shmget(key, 32, IPC_CREAT | 0666);
    if (shid < 0) {
        printf("Creation error in sender process: %s\n", strerror(errno));
        exit(-1);
    }

    void *shptr = shmat(shid, NULL, 0);
    if (shptr == (void *) -1) {
        printf("Attach error in sender process: %s\n", strerror(errno));
        exit(-1);
    }

    struct shmid_ds tmp;
    shmctl(shid, IPC_STAT, &tmp);
    if (tmp.shm_nattch > 1) {
        printf("%s\n", "Sender already exists");
        exit(-1);
    }

    int semid = semget(key, 1, IPC_CREAT);
    if (semid == -1) {
        printf("Semaphore creation error in sender process: %s\n", strerror(errno));
        exit(-1);
    }
    // ---------

    while ('-') {
        semop(semid, &sem_lock, 1);
        data to_send;
        to_send.time = time(NULL);
        to_send.pid = getpid();
        memcpy(shptr, &to_send, sizeof(data));
        semop(semid, &sem_open, 1);
    }

}

