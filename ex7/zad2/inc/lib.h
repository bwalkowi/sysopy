#ifndef _LIB_H_
#define _LIB_H_

#define BUF_SIZE 100

typedef struct{
    volatile unsigned int waiting_readers;
    volatile unsigned int working_readers;
    volatile unsigned int waiting_writers;
    volatile unsigned int working_writers;
    sem_t writers;
    sem_t readers;
    sem_t var;
    sem_t write;
    int buf[BUF_SIZE];
} library_t;


int lib_create(const char *name);
int lib_init(library_t *lib);
int lib_deinit(library_t *lib);
library_t *lib_open(const char *name);
int lib_close(library_t *lib);
int lib_destroy(const char *name);


#endif // _LIB_H_
