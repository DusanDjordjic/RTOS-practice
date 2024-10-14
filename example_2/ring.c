
#include "ring.h"
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>

int ring_init(ring_t* ring, unsigned long int size) {
    ring->size = size;

    if (ring->size == 0) {
        return 1;
    }

    ring->buffer = malloc(sizeof(unsigned int)  * size);
    if (ring->buffer == NULL) {
        return 2;
    }

    int res = sem_init(&(ring->read_sem), 0, 0);
    if (res != 0) {
        return 3;
    }

    res = sem_init(&(ring->write_sem), 0, size);
    if (res != 0) {
        return 4;
    }
    
    pthread_mutex_init(&(ring->done_lock), NULL);
    pthread_mutex_init(&(ring->read_lock), NULL);
    pthread_mutex_init(&(ring->write_lock), NULL);
    ring->read_index = 0;
    ring->write_index = 0;
    ring->done = 0;
    return 0;
}

int ring_done(ring_t* ring) {
    pthread_mutex_lock(&(ring->done_lock));
    sem_post(&(ring->read_sem));
    ring->done += 1;
    pthread_mutex_unlock(&(ring->done_lock));
    return 0;
}

int ring_destory(ring_t* ring) {
    free(ring->buffer);
    int res_1 = sem_destroy(&(ring->read_sem));
    int res_2 = sem_destroy(&(ring->write_sem));

    return res_1 != 0 || res_2 != 0;
}


int ring_write(ring_t* ring, unsigned int v) {
    int res = sem_wait(&(ring->write_sem));

    if (res != 0) {
        fprintf(stderr, "failed to wait for write");
        return 1;
    }
   
    pthread_mutex_lock(&(ring->write_lock));
    ring->buffer[ring->write_index] = v;
    ring->write_index = (ring->write_index + 1) % ring->size;
    sem_post(&(ring->read_sem));
    pthread_mutex_unlock(&(ring->write_lock));

    return 0; 
}

ring_action ring_read(ring_t* ring, unsigned int* v) {
    // if (v == NULL) {
    //     return ERROR;
    // }

    int res = sem_wait(&(ring->read_sem));
    
    pthread_mutex_lock(&(ring->done_lock));
    if (ring->done > 0) {
        int x = 0;
        sem_getvalue(&(ring->read_sem), &x);
        // printf("THREAD: %lu X: %d Done: %d\n", pthread_self(), x, ring->done);
        // fflush(stdout);
        if (x < ring->done) {
            // printf("THREAD: %lu X: %d DEC Done: %d\n", pthread_self(), x, ring->done);
            // fflush(stdout);
            ring->done--;
            pthread_mutex_unlock(&(ring->done_lock));
            return DONE;
        }
    }
    pthread_mutex_unlock(&(ring->done_lock));
    
    if (res != 0) {
        fprintf(stderr, "failed to wait for read");
        return ERROR;
    }
   
    pthread_mutex_lock(&(ring->read_lock));
    *v = ring->buffer[ring->read_index];
    ring->read_index = (ring->read_index + 1) % ring->size;
    sem_post(&(ring->write_sem));
    pthread_mutex_unlock(&(ring->read_lock));

    return OK; 
}

