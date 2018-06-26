#define _XOPEN_SOURCE 500

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

#include <pthread.h>
#include <unistd.h>

#define START   1
#define LAND    2


typedef struct{
    int capacity;
    int k;
    volatile int on_board;
    volatile int wanting_to_start;
    volatile int wanting_to_land;
    volatile bool airstrip_free;
    pthread_mutex_t mtx;
    pthread_cond_t starting;
    pthread_cond_t landing;
} aircraft_carrier_t;

typedef struct{
    int id;
    int time_to_sleep;
    volatile bool *alive;
    aircraft_carrier_t *ac;
} plane_t;

int get_approval(plane_t *plane, int action){
    int retval = 0;
    pthread_mutex_lock(&plane->ac->mtx);
    if(action == START){
        ++plane->ac->wanting_to_start;
        while(!plane->ac->airstrip_free)
            pthread_cond_wait(&plane->ac->starting, &plane->ac->mtx);
        plane->ac->airstrip_free = false;
        --plane->ac->wanting_to_start;
    }
    else if(action == LAND){
        ++plane->ac->wanting_to_land;
        while(!plane->ac->airstrip_free || plane->ac->on_board == plane->ac->capacity)
            pthread_cond_wait(&plane->ac->landing, &plane->ac->mtx);
        plane->ac->airstrip_free = false;
        --plane->ac->wanting_to_land;
    }
    else
        retval = EINVAL;

    pthread_mutex_unlock(&plane->ac->mtx);
    return retval;
}

void free_airstrip(aircraft_carrier_t *ac){
    if(ac->on_board < ac->k){
        if(ac->wanting_to_land > 0)
            pthread_cond_signal(&ac->landing);
        else
            pthread_cond_signal(&ac->starting);
    }
    else{
        if(ac->wanting_to_start > 0)
            pthread_cond_signal(&ac->starting);
        else if(ac->on_board < ac->capacity)
            pthread_cond_signal(&ac->landing);
    }
}

void start(plane_t *plane){
    pthread_mutex_lock(&plane->ac->mtx);

    --plane->ac->on_board;
    plane->ac->airstrip_free = true;
    free_airstrip(plane->ac);

    pthread_mutex_unlock(&plane->ac->mtx);
}

void land(plane_t *plane){
    pthread_mutex_lock(&plane->ac->mtx);

    ++plane->ac->on_board;
    plane->ac->airstrip_free = true;
    free_airstrip(plane->ac);

    pthread_mutex_unlock(&plane->ac->mtx);
}

void *thread_func(void *arg){
    plane_t *plane = arg;
    while(*plane->alive){
        get_approval(plane, LAND);
        land(plane);
        printf("Plane %d: landed\n", plane->id);

        usleep(plane->time_to_sleep);

        get_approval(plane, START);
        start(plane);
        printf("Plane %d: started\n", plane->id);

        usleep(plane->time_to_sleep);
    }
    return NULL;
}

int main(void){
    srand(time(NULL));
    int max_sleep = 1000000;
    int plane_num = 37;
    int capacity = 10;
    int k = 5;
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

    aircraft_carrier_t ac = {.capacity = capacity, .k = k, .on_board = 0, .wanting_to_start = 0, .wanting_to_land = 0, .airstrip_free = true};

    pthread_t thread_pool[plane_num];
    plane_t planes[plane_num];
    volatile bool alive = true;

    if((err = pthread_mutex_init(&ac.mtx, NULL))  != 0 || (err = pthread_cond_init(&ac.starting, NULL))  != 0 || (err = pthread_cond_init(&ac.landing, NULL))  != 0 ){
        printf("init mutex/cond: %s\n", strerror(err));
        goto cleanup;
    }

    for(int i = 0; i < plane_num; ++i){
        planes[i].id = i;
        planes[i].time_to_sleep = rand() % max_sleep;
        planes[i].alive = &alive;
        planes[i].ac = &ac;
        
        if((err = pthread_create(&thread_pool[i], NULL, thread_func, &planes[i]))){
            printf("creating thread failed: %s\n", strerror(err));
            goto cleanup;
        }
    }

    int sig;
    (void)sigwait(&set, &sig);

    cleanup:
        alive = false;
        for(int i = 0; i < plane_num; ++i)
            (void)pthread_join(thread_pool[i], NULL);

        (void)pthread_mutex_destroy(&ac.mtx);
        (void)pthread_cond_destroy(&ac.starting);
        (void)pthread_cond_destroy(&ac.landing);

        if(err)
            return -1;
        else
            return 0;
}

