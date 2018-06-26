#define _XOPEN_SOURCE 500

#include <stdlib.h>

#include <sys/mman.h>
#include <unistd.h>
#include <semaphore.h>

#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>

#include "lib.h"


int lib_create(const char *name){
    int shmid = shm_open(name, O_RDWR | O_CREAT | O_EXCL, S_IRWXU | S_IRWXG | S_IRWXO);
    if(shmid == -1)
        return -1;

    int retval = ftruncate(shmid, sizeof(library_t));
    if(retval == -1)
        lib_destroy(name);

    return retval;
}

int lib_init(library_t *lib){
    int retval = sem_init(&lib->writers, 1, 0) | sem_init(&lib->readers, 1, 0) | sem_init(&lib->var, 1, 1) | sem_init(&lib->write, 1, 1);
    if(retval == -1)
        lib_deinit(lib);
    return retval;
}

int lib_deinit(library_t *lib){
    return sem_destroy(&lib->writers) | sem_destroy(&lib->readers) | sem_destroy(&lib->var) | sem_destroy(&lib->write);
}

library_t *lib_open(const char *name){
    int shmid = shm_open(name, O_RDWR, S_IRWXU | S_IRWXG | S_IRWXO);
    if(shmid == -1)
        return NULL;
    library_t *lib = mmap(NULL, sizeof(*lib), PROT_READ | PROT_WRITE, MAP_SHARED, shmid, 0);
    return lib == MAP_FAILED ? NULL : lib;
}

int lib_close(library_t *lib){
    return munmap(lib, sizeof(*lib));
}

int lib_destroy(const char *name){
    return shm_unlink(name);
}

