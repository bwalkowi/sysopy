#define _XOPEN_SOURCE

#include "shm.h"
#include "sem.h"

#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <signal.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <unistd.h>


static void sigint_handler(int sig){
    return;
}

static int sem_init(hangar_t *han){
    han->full = sem_get(0);
    han->free = sem_get(BUF_SIZE);
    han->i_guard = sem_get(1);
    han->j_guard = sem_get(1);
    if(han->full == -1 || han->free == -1 || han->i_guard == -1 || han->j_guard == -1)
        return -1;

    return 0;
}

static int sem_destroy(hangar_t *han){
    return sem_remove(han->full) | sem_remove(han->free) | sem_remove(han->i_guard) | sem_remove(han->j_guard);
}

int main(int argc, char **argv){
    if(argc != 3){
        printf("Incorrect arguments. Required ones: prog n m\n"
               "\tprog - program name\n"
               "\tpath - path used in ftok\n"
               "\tid - id used in ftok\n");
        return -1;
	}

    signal(SIGINT, sigint_handler);

    int shmid;
    hangar_t *han = shm_get(argv[1], argv[2][0], &shmid);
    if(han == NULL){
        perror("creating shared memory");
        return -1;
    }

    if(sem_init(han) == -1){
        perror("init semaphores");
        shmdt(han);
        shmctl(shmid, IPC_RMID, NULL);
    }

    pause();

    int retval = sem_destroy(han);
    if(retval == -1)
        perror("destroy semaphores");

    if(shmdt(han) == -1 || shmctl(shmid, IPC_RMID, NULL) == -1){
        perror("deleting shared memory");
        retval = -1;
    }

    return retval;
}

