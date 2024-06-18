#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <math.h>
#include <semaphore.h>

#define BUFFER_SIZE 100

int buffer[BUFFER_SIZE];
int pindex = 0;
int cindex = 0;
int count = 0;
pthread_mutex_t mutex;
pthread_cond_t empty_cv;
pthread_cond_t full_cv;

int produce( int id ) {
    int r = rand();
    printf("Producer %d produced %d.\n", id, r);
    return r;
}

void consume( int id, int number ) {
    printf("Consumer %d consumed %d.\n", id, number);
}

void* producer( void* arg ) {
    int* id = (int*) arg;
    for(int i = 0; i < 10000; ++i) {
        int num = produce(*id); 
        pthread_mutex_lock( &mutex );
        while( count == BUFFER_SIZE) {
            pthread_cond_wait( &full_cv, &mutex );
        }
        count++;
        buffer[pindex] = num;
        pindex = (pindex + 1) % BUFFER_SIZE;
        pthread_cond_signal( &empty_cv );
        pthread_mutex_unlock( &mutex );
    }
    free( arg );
    pthread_exit( NULL );
}

void* consumer( void* arg ) {
    int* id = (int*) arg;
    for(int i = 0; i < 10000; ++i) {
        pthread_mutex_lock( &mutex );
        while( count == 0) {
            pthread_cond_wait( &empty_cv, &mutex );
        }
        count--;
        int num = buffer[cindex];
        buffer[cindex] = -1;
        cindex = (cindex + 1) % BUFFER_SIZE;
        pthread_cond_signal( &full_cv );
        pthread_mutex_unlock( &mutex );
        consume( *id, num );
    }
    free( id );
    pthread_exit( NULL );
}

int main( int argc, char** argv ) {
    pthread_mutex_init( &mutex, NULL );
    pthread_cond_init ( &full_cv, NULL );
    pthread_cond_init ( &empty_cv, NULL );

    pthread_t threads[20];

    for( int i = 0; i < 10; i++ ) {
        int* id = malloc(sizeof(int));
        *id = i;
        pthread_create(&threads[i], NULL, producer, id);
    }
    for( int j = 10; j < 20; j++ ) {
        int* jd = malloc(sizeof(int));
        *jd = j-10;
        pthread_create(&threads[j], NULL, consumer, jd);
    }
    for( int k = 0; k < 20; k++ ){  
        pthread_join(threads[k], NULL);
    }
    pthread_mutex_destroy( &mutex );
    pthread_cond_destroy( &full_cv );
    pthread_cond_destroy( &empty_cv );
    pthread_exit(0);
}
