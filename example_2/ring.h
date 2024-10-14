#ifndef RING_H
#define RING_H

#include <pthread.h>
#include <semaphore.h>

typedef enum { OK = 0, DONE, ERROR } ring_action;

typedef struct {
    unsigned long int size; 
    unsigned int* buffer;
    unsigned int read_index;
    unsigned int write_index;
    sem_t read_sem;
    sem_t write_sem;
    unsigned char done;
    pthread_mutex_t done_lock;
    pthread_mutex_t read_lock;
    pthread_mutex_t write_lock;
} ring_t;

int ring_init(ring_t* ring, unsigned long int size);
int ring_destory(ring_t* ring);
int ring_write(ring_t* ring, unsigned int v);
ring_action ring_read(ring_t* ring, unsigned int* v);
int ring_done(ring_t* ring);

#endif 
