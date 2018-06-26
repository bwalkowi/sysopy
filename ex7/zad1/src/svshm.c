#define _XOPEN_SOURCE

#include <stdlib.h>

#include "shm.h"

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/ipc.h>


hangar_t *shm_get(const char *path, char id, int *shmid){

    key_t key = ftok(path, id);
    if(key == -1)
        return NULL;

    *shmid = shmget(key, sizeof(hangar_t), IPC_CREAT | S_IRWXU | S_IRWXG | S_IRWXO);
    if(*shmid == -1)
        return NULL;

    hangar_t *han = shmat(*shmid, NULL, 0);
    if(han == (hangar_t *)-1)
        return NULL;

    return han;
}

