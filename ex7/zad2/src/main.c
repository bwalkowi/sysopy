#define _XOPEN_SOURCE

#include <errno.h>
#include <stdlib.h>
#include <signal.h>
#include <stdio.h>

#include <semaphore.h>
#include <unistd.h>

#include "lib.h"


static void sigint_handler(int sig){
    return;
}

int main(int argc, char **argv){
    if(argc != 2){
        printf("Incorrect arguments. Required ones: prog name\n"
               "\tprog - program name\n"
               "\tname - posix name used to create shm\n");
        return -1;
	}

    signal(SIGINT, sigint_handler);

    int lib_id = lib_create(argv[1]);
    if(lib_id == -1){
        perror("creating lib");
        return -1;
    }

    library_t *lib = lib_open(argv[1]);
    if(lib == NULL || lib_init(lib) == -1){
        perror("initing lib");
        lib_destroy(argv[1]);
        return -1;
    }

    pause();

    int retval = 0;
    if(lib_deinit(lib) == -1 || lib_close(lib) == -1){
        perror("deinitint lib");
        retval = -1;
    }

    if(lib_destroy(argv[1]) == -1){
        perror("destroying lib");
        retval = -1;
    }

    return retval;
}

