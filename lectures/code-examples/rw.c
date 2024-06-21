#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "rwlock.h"

#define BUFFER_SIZE 100000
#define NUM_READERS 10
#define NUM_WRITERS 5


unsigned int r_sleep = 15000;
unsigned int w_sleep = 5000;

int buffer[BUFFER_SIZE];

rwlock_t rwlock;

void read_buff(int id) {
    int sum = 0;
    for(int i = 0; i < BUFFER_SIZE; i++) {
        sum += buffer[i];
    }
    printf("Reader %d read %d.\n", id, sum);
}

void write_buff(int id, int index, int add) {
    int tmp = buffer[index] + add;
    buffer[index] = tmp;
    printf("Writer %d wrote %d.\n", id, tmp);
}

void* reader( void* arg ) {
    int* id = (int*) arg;
    for(int i = 0; i < 20; i++) {
        rwlock_r_lock(&rwlock);
        printf("Reader %d entered CS.\n", *id);
        read_buff(*id);
        rwlock_r_unlock(&rwlock);
        printf("Reader %d exited CS\n", *id);
        usleep(r_sleep);
    }
    free( id );
    pthread_exit( NULL );
}

void* writer( void* arg ) {
    int* id = (int*) arg;
    for(int i = 0; i < 60; i++) {
        rwlock_w_lock(&rwlock);
        printf("Writer %d entered CS.\n", *id);
        write_buff(*id, 0, 1);
        rwlock_w_unlock(&rwlock);
        printf("Writer %d exited CS\n", *id);
        usleep(w_sleep);
    }
    free( id );
    pthread_exit( NULL );
}

int main(int argc, char** argv) {
    rwlock_init(&rwlock);
    memset(buffer, 0, BUFFER_SIZE * sizeof(int));
    pthread_t threads[NUM_READERS + NUM_WRITERS];

    for(int i = 0; i < NUM_READERS; i++) {
        int* id = malloc(sizeof(int));
        *id = i;
        pthread_create(&threads[i], NULL, reader, id);
    }

    for(int i = 0; i < NUM_WRITERS; i++) {
        int* id = malloc(sizeof(int));
        *id = i;
        pthread_create(&threads[i + NUM_READERS], NULL, writer, id);
    }

    for(int i = 0; i < NUM_READERS + NUM_WRITERS; i++){
        pthread_join(threads[i], NULL);
    }

    rwlock_destroy(&rwlock);
    pthread_exit(0);
}
