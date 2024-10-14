#include<stdio.h>
#include<pthread.h>
#include<semaphore.h>
#include<unistd.h>

#include "ring.h"

typedef struct {
    ring_t* in;
    ring_t* out;
} consumer_args;


void* double_worker(void* arguments) {
    consumer_args* args = (consumer_args*) arguments;
    unsigned int data;
    int res;
    while ((res = ring_read(args->in, &data)) != DONE) {
        if (res == ERROR) {
            fprintf(stderr, "failed to read from ring %d\n", res);
            continue;
        }

        res = ring_write(args->out, data * 2);
        if (res != 0) {
            fprintf(stderr, "failed to write to out ring");
        }
    }

    return NULL;
}

void* print_worker(void* arguments) {
    consumer_args* args = (consumer_args*) arguments;
    unsigned int data = 0;
    int res;
    while ((res = ring_read(args->in, &data)) != DONE) {
        if (res == ERROR) {
            fprintf(stderr, "failed to read from ring %d\n", res);
            continue;
        }

        printf("RES: %u\n", data);
        fflush(stdout);
    }

    return NULL;
}

void* producer(void* arguments) {
    consumer_args* args = (consumer_args*) arguments;

    for (unsigned int i = 0; i < 1000000; i++) {
        int res = ring_write(args->out, i);
        if (res != 0) {
            fprintf(stderr, "failed to write to ring %d\n", res);
        }
    }

    return NULL;
}

int main() {
    ring_t input;
    int res = ring_init(&input, 150);
    if (res != 0) {
        fprintf(stderr, "failed to init ring buffer %d\n", res);
        return 1;
    }

    ring_t output;
    res = ring_init(&output, 100);
    if (res != 0) {
        fprintf(stderr, "failed to init ring buffer %d\n", res);
        return 1;
    }
    
    pthread_t producer_1, producer_2, producer_3;
    consumer_args producer_args = {.in = NULL, .out = &input};
    pthread_create(&producer_1, NULL, producer, (void*) &producer_args);
    pthread_create(&producer_2, NULL, producer, (void*) &producer_args);
    pthread_create(&producer_3, NULL, producer, (void*) &producer_args);
    
    pthread_t double_worker_thread_1, double_worker_thread_2;
    consumer_args double_args = {.in = &input, .out = &output};
    pthread_create(&double_worker_thread_1, NULL, double_worker, (void*) &double_args);
    pthread_create(&double_worker_thread_2, NULL, double_worker, (void*) &double_args);

    pthread_t print_worker_thread_1, print_worker_thread_2, print_worker_thread_3, print_worker_thread_4; 
    consumer_args print_worker_args = {.in = &output, .out = NULL};
    pthread_create(&print_worker_thread_1, NULL, print_worker, (void*) &print_worker_args);
    pthread_create(&print_worker_thread_2, NULL, print_worker, (void*) &print_worker_args);
    pthread_create(&print_worker_thread_3, NULL, print_worker, (void*) &print_worker_args);
    pthread_create(&print_worker_thread_4, NULL, print_worker, (void*) &print_worker_args);
   
    pthread_join(producer_1, NULL);
    pthread_join(producer_2, NULL);
    pthread_join(producer_3, NULL);
   
    ring_done(&input);
    ring_done(&input);
    pthread_join(double_worker_thread_1, NULL);
    pthread_join(double_worker_thread_2, NULL);

    ring_done(&output);
    ring_done(&output);
    ring_done(&output);
    ring_done(&output);
    pthread_join(print_worker_thread_1, NULL);
    pthread_join(print_worker_thread_2, NULL);
    pthread_join(print_worker_thread_3, NULL);
    pthread_join(print_worker_thread_4, NULL);

    ring_destory(&input);
    ring_destory(&output);

    return 0;
}
