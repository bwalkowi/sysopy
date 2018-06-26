#define _XOPEN_SOURCE 500

#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <signal.h>
#include <errno.h>

#include <semaphore.h>

#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include "lib.h"


static unsigned long found_num = 0;
static bool terminated = false;

static void sigint_handler(int p) {
    terminated = true;
}

static void reader_read(library_t *lib, int num, bool u){
    struct timeval te;
    gettimeofday(&te, NULL);

    int tmp = 0;
    for(int i = 0; i < BUF_SIZE; ++i)
        if(lib->buf[i] == num)
            ++tmp;

    if(u)
        found_num += tmp;
    else
        printf("(%ld - %lld) Znalazłem %d liczb o wartości %d.\n", (long)getpid(), te.tv_sec*1000LL + te.tv_usec/1000, tmp, num);
}

int main(int argc, char **argv){

    if(argc < 3 || argc > 4){
        printf("Incorrect arguments. Required ones: prog path x [u]\n"
               "\tprog - program name\n"
               "\tpath - posix requirement\n"
               "\tx - number to search for\n"
               "\tu - additional arg\n");
        return -1;
	}

    errno = 0;
    long num = strtol(argv[2], NULL, 10);
    if(errno){
        perror("offset must be a nonegative number");
        return -1;
    }

    bool u = (argc == 4 && argv[3][0] == 'u' && argv[3][1] == '\0') ? true : false;

    signal(SIGINT, sigint_handler);

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

    library_t *lib = lib_open(argv[1]);
    if(lib == NULL){
        perror("open lib");
        return -1;
    }

    while(!terminated){
        sigprocmask(SIG_SETMASK, &procmask, NULL);
        
        sem_wait(&lib->var);

        ++lib->waiting_readers;
        if(lib->waiting_writers == 0)
            while(lib->working_readers < lib->waiting_readers){
                ++lib->working_readers;
                sem_post(&lib->readers);
            }

        sem_post(&lib->var);

        sem_wait(&lib->readers);

        reader_read(lib, num, u);

        sem_wait(&lib->var);

        --lib->working_readers;
        --lib->waiting_readers;
        if(lib->working_readers == 0)
            while(lib->working_writers < lib->waiting_writers){
                ++lib->working_writers;
                sem_post(&lib->writers);
            }

        sem_post(&lib->var);

        sigprocmask(SIG_SETMASK, &oprocmask, NULL);
    }

    if(u)
        printf("%ld Znalazłem %lu liczb o wartości %ld.\n", (long)getpid(), found_num, num);

    return lib_close(lib);
}

