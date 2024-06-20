#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <math.h>
#include "fifo.h"

#define BUFFER_SIZE 100
#define NUM_PRODUCERS 10
#define NUM_CONSUMERS 10

int buffer[BUFFER_SIZE];
int p_index = 0;
int c_index = 0;
int count = 0;

int p_turn = 0;
int c_turn = 0;
int next_p_turn = 0;
int next_c_turn = 0;

pthread_mutex_t mutex;
pthread_cond_t full_cv[NUM_PRODUCERS];
pthread_cond_t empty_cv[NUM_CONSUMERS];

fifo_t p_fifo;
fifo_t c_fifo;

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
    for(int i = 0; i < 10000; i++) {
        int num = produce(*id); 
        pthread_mutex_lock(&mutex);
        int my_turn = p_turn++;
        fifo_push(&p_fifo, &full_cv[*id]);
        while(count == BUFFER_SIZE || next_p_turn < my_turn) {
            pthread_cond_wait( &full_cv[*id], &mutex );
        }
        buffer[p_index] = num;
        p_index = (p_index + 1) % BUFFER_SIZE;
        count++;
        fifo_pop(&p_fifo);
        next_p_turn++;
        pthread_cond_t *c_fifo_head = (pthread_cond_t*) fifo_head(&c_fifo);
        if(c_fifo_head != NULL) {
            pthread_cond_signal(c_fifo_head);
        }
        pthread_mutex_unlock( &mutex );
    }
    free( arg );
    pthread_exit( NULL );
}

void* consumer( void* arg ) {
    int* id = (int*) arg;
    for(int i = 0; i < 10000; i++) {
        pthread_mutex_lock( &mutex );
        int my_turn = c_turn++;
        fifo_push(&c_fifo, &empty_cv[*id]);
        while(count == 0 || next_c_turn < my_turn) {
            pthread_cond_wait( &empty_cv[*id], &mutex );
        }
        int num = buffer[c_index];
        buffer[c_index] = -1;
        c_index = (c_index + 1) % BUFFER_SIZE;
        count--;
        fifo_pop(&c_fifo);
        next_c_turn++;
        pthread_cond_t *p_fifo_head = (pthread_cond_t*) fifo_head(&p_fifo);
        if(p_fifo_head != NULL) {
            pthread_cond_signal(p_fifo_head);
        }
        pthread_mutex_unlock( &mutex );
        consume( *id, num );
    }
    free( id );
    pthread_exit( NULL );
}

int main( int argc, char** argv ) {
    pthread_mutex_init( &mutex, NULL );

    for( int i = 0; i < NUM_PRODUCERS; i++ ) {
        pthread_cond_init ( &full_cv[i], NULL );
    }
    for( int i = 0; i < NUM_CONSUMERS; i++ ) {
        pthread_cond_init ( &empty_cv[i], NULL );
    }

    fifo_init(&p_fifo, NUM_PRODUCERS);
    fifo_init(&c_fifo, NUM_CONSUMERS);

    pthread_t threads[NUM_PRODUCERS + NUM_CONSUMERS];

    for( int i = 0; i < NUM_PRODUCERS; i++ ) {
        int* id = malloc(sizeof(int));
        *id = i;
        pthread_create(&threads[i], NULL, producer, id);
    }

    for( int i = 0; i < NUM_CONSUMERS; i++ ) {
        int* id = malloc(sizeof(int));
        *id = i;
        pthread_create(&threads[i + NUM_PRODUCERS], NULL, consumer, id);
    }

    for( int i = 0; i < NUM_PRODUCERS + NUM_CONSUMERS; i++ ){
        pthread_join(threads[i], NULL);
    }

    pthread_mutex_destroy( &mutex );

    for( int i = 0; i < NUM_CONSUMERS; i++ ) {
        pthread_cond_destroy(&full_cv[i]);
    }

    for( int i = 0; i < NUM_CONSUMERS; i++ ) {
        pthread_cond_destroy(&empty_cv[i]);
    }

    fifo_destroy(&p_fifo);
    fifo_destroy(&c_fifo);

    pthread_exit(0);
}
