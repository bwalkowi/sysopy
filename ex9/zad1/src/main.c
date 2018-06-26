#define _XOPEN_SOURCE 500

#include <stdio.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>


typedef struct{
    int id;
    volatile bool *alive;
    sem_t *fork1;
    sem_t *fork2;
} philosopher_t;

void *thread_func(void *arg){
    philosopher_t *philosopher = arg;
    while(*philosopher->alive){
        usleep(100000);
        sem_wait(philosopher->fork1);
        sem_wait(philosopher->fork2);

        printf("philosopher %d: eating\n", philosopher->id);

        sem_post(philosopher->fork1);
        sem_post(philosopher->fork2);
    }
    return NULL;
}

int main(void){
    int num = 5;
    int err = 0;
    sigset_t set;

    if(sigfillset(&set) == -1){
        perror("creating sigmask");
        return -1;
    }
    if( (err = pthread_sigmask(SIG_SETMASK, &set, NULL)) != 0){
        printf("setting sigmask: %s\n", strerror(err));
        return -1;
    }

    sem_t semaphores[num];
    pthread_t thread_pool[num];
    philosopher_t philosophers[num];
    volatile bool alive = true;

    for(int i = 0; i < num; ++i)
        if(sem_init(&semaphores[i], 0, 1) == -1){
            perror("init sem");
            goto cleanup;
        }

    for(int i = 0; i < num - 1; ++i){
        philosophers[i].id = i;
        philosophers[i].alive  = &alive;
        philosophers[i].fork1 = &semaphores[i];
        philosophers[i].fork2 = &semaphores[i + 1];

        if((err = pthread_create(&thread_pool[i], NULL, thread_func, &philosophers[i]))){
            printf("creating thread failed: %s\n", strerror(err));
            goto cleanup;
        }
    }

    philosophers[num - 1].id = num - 1;
    philosophers[num - 1].alive = &alive;
    philosophers[num - 1].fork1 = &semaphores[0];
    philosophers[num - 1].fork2 = &semaphores[num - 1];
    if((err = pthread_create(&thread_pool[num - 1], NULL, thread_func, &philosophers[num - 1]))){
        printf("creating thread failed: %s\n", strerror(err));
        goto cleanup;
    }

    int sig;
    (void)sigwait(&set, &sig);

    alive = false;
    for(int i = 0; i < num; ++i)
        (void)pthread_join(thread_pool[i], NULL);
    for(int i = 0; i < num; ++i)
        (void)sem_destroy(&semaphores[i]);

    return 0;

    cleanup:
        alive = false;
        for(int i = 0; i < num; ++i){
            (void)pthread_join(thread_pool[i], NULL);
            (void)sem_destroy(&semaphores[i]);
        }
        return -1;
}

