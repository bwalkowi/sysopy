#define _XOPEN_SOURCE

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <time.h>

#include <semaphore.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include "lib.h"


static bool terminated = false;

static void sigint_handler(int p) {
    terminated = true;
}

static int randint(int limit){
    return rand() % limit;
}

static bool in(int num, int *arr, int len){
    for(int i = 0; i < len; ++i)
        if(arr[i] == num)
            return true;

    return false;
}

static void writer_write(library_t *lib, int num, int *indexes, int *values, bool *first_time){
    struct timeval te;
    gettimeofday(&te, NULL);

    if(*first_time)
        for(int i = 0; i < num; ++i){
            lib->buf[indexes[i]] = values[i];
            printf("(%ld - %lld) Wpisałem liczbę: %d na pozycji %d. Pozostało %d zadan.\n", (long)getpid(), te.tv_sec*1000LL + te.tv_usec/1000, lib->buf[indexes[i]], indexes[i], num - i - 1);
            *first_time = false;
        }

    else
        for(int i = 0; i < num; ++i){
            ++lib->buf[indexes[i]];
            printf("(%ld - %lld) Wpisałem liczbę: %d na pozycji %d. Pozostało %d zadan.\n", (long)getpid(), te.tv_sec*1000LL + te.tv_usec/1000, lib->buf[indexes[i]], indexes[i], num - i - 1);
        }
}

int main(int argc, char **argv){
    if(argc != 2){
        printf("Incorrect arguments. Required ones: prog path\n"
               "\tprog - program name\n"
               "\tpath - posix requirement\n");
        return -1;
	}

    sigset_t procmask;
    sigset_t oprocmask;

    if(sigfillset(&procmask) == -1){
        perror("creating proc mask");
        return -1;
    }

    if(sigprocmask(SIG_SETMASK, NULL, &oprocmask) == -1){
        perror("getting old procmask");
        return -1;
    }

    signal(SIGINT, sigint_handler);

    library_t *lib = lib_open(argv[1]);
    if(lib == NULL){
        perror("open lib");
        return -1;
    }

    srand(time(NULL));

    int num = randint(BUF_SIZE);
    int indexes[num];
    int values[num];

    int index= randint(BUF_SIZE);
    for(int i = 0; i < num; ++i){
        while(in(index, indexes, i))
            index = randint(BUF_SIZE);

        indexes[i] = index;
        values[i] = randint(2000000);
    }

    bool first_time = true;
    while(!terminated){
        sigprocmask(SIG_SETMASK, &procmask, NULL);

        sem_wait(&lib->var);

        ++lib->waiting_writers;
        if(lib->waiting_readers == 0)
            while(lib->working_writers < lib->waiting_writers){
                ++lib->working_writers;
                sem_post(&lib->writers);
            }

        sem_post(&lib->var);

        sem_wait(&lib->writers);
        sem_wait(&lib->write);

        writer_write(lib, num, indexes, values, &first_time);

        sem_post(&lib->write);
        sem_wait(&lib->var);

        --lib->working_writers;
        --lib->waiting_writers;
        if(lib->working_writers == 0)
            while(lib->working_readers < lib->waiting_readers){
                ++lib->working_readers;
                sem_post(&lib->readers);
            }

        sem_post(&lib->var);

        sigprocmask(SIG_SETMASK, &oprocmask, NULL);
    }

    return lib_close(lib);
}

