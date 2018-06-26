#ifndef _SHM_H_
#define _SHM_H_

#define BUF_SIZE 100

#include <stdatomic.h>

typedef struct{
    volatile atomic_int in_stock;
    int full;
    int free;
    volatile int i;
    int i_guard;
    volatile int j;
    int j_guard;
    int buf[BUF_SIZE];
} hangar_t;

hangar_t *shm_get(const char *path, char id, int *shmid);


#endif // _SHM_H_
