#include<stdio.h>
#include<pthread.h>
#include<semaphore.h>

typedef struct {
    int* counter;
    sem_t* sem;
    sem_t* done;
    sem_t* read;
} count_args;

void count(void* arguments) {
    count_args* args =  (count_args*) arguments;
    while(1) {
        int res = sem_trywait(args->done);
        if (res == 0)  {
            printf("DONE\n");
            break;
        }

        res = sem_wait(args->sem);
        if (res != 0) {
            fprintf(stderr, "faiting for sem failed %d\n", res);
            return;
        }

        sem_post(args->read);

        *(args->counter)+= 1;
        printf("counter = %d\n", *(args->counter));
        fflush(stdout);
    }
}

int main() {
    sem_t semaphore;
    int ret = sem_init(&semaphore, 0, 0);
    if (ret != 0) {
        fprintf(stderr, "failed to init sem");
        return 1;
    }

    sem_t done;
    ret = sem_init(&done, 0, 0);
    if (ret != 0) {
        fprintf(stderr, "failed to init done sem");
        return 1;
    }

    sem_t read;
    ret = sem_init(&read, 0, 0);
    if (ret != 0) {
        fprintf(stderr, "failed to init read sem");
        return 1;
    }
    
    int counter = 0;
    count_args args = {.counter = &counter, .sem = &semaphore, .done = &done, .read = &read};

    pthread_t thread;
    pthread_create(&thread, NULL, (void*)count, (void*) &args);

    while(1) {
        char c = getchar();
        if (c == 'q' || c == 'Q') {
            break;
        }

        if (c == '\n') {
            continue;
        }


        sem_post(&semaphore);
        sem_wait(&read);
    }

    sem_post(&semaphore);
    sem_post(&done);
    pthread_join(thread, NULL);
    sem_destroy(&semaphore);
    sem_destroy(&done);
    sem_destroy(&read);

    return 0;
}
