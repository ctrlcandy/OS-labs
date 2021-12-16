#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>

int counter = 0;
pthread_rwlock_t rwlock;

void *to_write(void *arg) {
    while ('^') {
        pthread_rwlock_wrlock(&rwlock);
        counter++;
        pthread_rwlock_unlock(&rwlock);
        sleep(1);
    }
}

void *to_read(void *arg) {
    while ('3') {
        pthread_rwlock_rdlock(&rwlock);
        printf("tid: %lu counter: %d\n", pthread_self(), counter);
        pthread_rwlock_unlock(&rwlock);
        sleep(1);
    }
}

int main(int argc, char **argv) {
    pthread_t read_threads[10];
    pthread_t write_thread;

    pthread_rwlock_init(&rwlock, NULL);
    pthread_create(&write_thread, NULL, &to_write, NULL);

    for (int i = 0; i < 10; ++i)
        pthread_create(&read_threads[i], NULL, &to_read, NULL);

    pthread_join(write_thread, NULL);
}
