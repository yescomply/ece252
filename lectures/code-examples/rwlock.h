#include <stdlib.h>
#include <pthread.h>

typedef struct {
    int ww;
    int wr;
    int aw;
    int ar;
    pthread_mutex_t mutex;
    pthread_cond_t ok2write;
    pthread_cond_t ok2read;
} rwlock_t;


void rwlock_init(rwlock_t *lock) {
    lock->ww = 0;
    lock->wr = 0;
    lock->aw = 0;
    lock->ar = 0;
    pthread_mutex_init(&lock->mutex, NULL);
    pthread_cond_init(&lock->ok2write, NULL);
    pthread_cond_init(&lock->ok2read, NULL);
}

void rwlock_destroy(rwlock_t *lock) {
    pthread_mutex_destroy(&lock->mutex);
    pthread_cond_destroy(&lock->ok2write);
    pthread_cond_destroy(&lock->ok2read);
}

void rwlock_r_lock(rwlock_t *lock) {
    pthread_mutex_lock(&lock->mutex);
    while(lock->aw + lock->ww > 0) {
        lock->wr = lock->wr + 1;
        pthread_cond_wait(&lock->ok2read, &lock->mutex);
        lock->wr = lock->wr - 1;
    }
    lock->ar = lock->ar + 1;
    pthread_mutex_unlock(&lock->mutex);
}

void rwlock_r_unlock(rwlock_t *lock) {
    pthread_mutex_lock(&lock->mutex);
    lock->ar = lock->ar - 1;
    if(lock->ar == 0 && lock->ww > 0) {
        pthread_cond_signal(&lock->ok2write);
    }
    pthread_mutex_unlock(&lock->mutex);
}

void rwlock_w_lock(rwlock_t *lock) {
    pthread_mutex_lock(&lock->mutex);
    while(lock->ar + lock->aw > 0) {
        lock->ww = lock->ww + 1;
        pthread_cond_wait(&lock->ok2write, &lock->mutex);
        lock->ww = lock->ww - 1;
    }
    lock->aw = lock->aw + 1;
    pthread_mutex_unlock(&lock->mutex);
}

void rwlock_w_unlock(rwlock_t *lock) {
    pthread_mutex_lock(&lock->mutex);
    lock->aw = lock->aw - 1;
    if(lock->ww > 0) {
        pthread_cond_signal(&lock->ok2write);
    } else if(lock->wr > 0) {
        pthread_cond_signal(&lock->ok2read);
    }
    pthread_mutex_unlock(&lock->mutex);
}