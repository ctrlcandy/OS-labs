#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>

pthread_mutex_t mutex;
pthread_cond_t cond;
int counter = 0;

void *to_write(void *arg) {
    while ('^') {
        pthread_mutex_lock(&mutex);
        counter++;
        pthread_mutex_unlock(&mutex);
        pthread_cond_signal(&cond);
        sleep(1);
    }
}

void *to_read(void *arg) {
    while ('3') {
        pthread_cond_wait(&cond, &mutex);
        printf("tid: %lu counter: %d\n", pthread_self(), counter);
        pthread_mutex_unlock(&mutex);
        sleep(2);
    }
}

int main(int argc, char **argv) {
    pthread_t read_threads[10];
    pthread_t write_thread;

    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);
    pthread_create(&write_thread, NULL, &to_write, NULL);

    for (int i = 0; i < 10; ++i)
        pthread_create(&read_threads[i], NULL, &to_read, NULL);

    pthread_join(write_thread, NULL);
}
