#define _XOPEN_SOURCE

#include "sem.h"

#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <unistd.h>

#include <stdio.h>


int sem_get(int val){
    union semun {
        int val;
        struct semid_ds *buf;
        unsigned short *array;
    } arg;
    int semid = -1;
    if( (semid = semget(IPC_PRIVATE, 1, S_IRWXU | S_IRWXG | S_IRWXO | IPC_CREAT | IPC_EXCL)) != -1){
        arg.val = val;
        if(semctl(semid, 0, SETVAL, arg) == -1)
            return -1;
    }

    return semid;
}

int sem_remove(int semid){
    return semctl(semid, 0, IPC_RMID);
}

int sem_up(int semid){
    struct sembuf sop;
    sop.sem_num = 0;
    sop.sem_op = 1;
    sop.sem_flg = 0;

    return semop(semid, &sop, 1);
}

int sem_down(int semid){
    struct sembuf sop;
    sop.sem_num = 0;
    sop.sem_op = -1;
    sop.sem_flg = 0;

    return semop(semid, &sop, 1);
}

