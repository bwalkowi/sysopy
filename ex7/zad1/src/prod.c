#define _XOPEN_SOURCE

#include "shm.h"
#include "sem.h"

#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <stdlib.h>

#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>


static int randint(int limit){
    return rand() % limit;
}

int main(int argc, char **argv){
    if(argc != 3){
        printf("Incorrect arguments. Required ones: prog n m\n"
               "\tprog - program name\n"
               "\tpath - path used in ftok\n"
               "\tid - id used in ftok\n");
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

    int shmid;
    hangar_t *han = shm_get(argv[1], argv[2][0], &shmid);
    if(han == NULL){
        perror("creating shared memory");
        return -1;
    }

    int num;
    int in_stock;
    int i;
    struct timeval te;
    srand(time(NULL));

    while(1){
        sigprocmask(SIG_SETMASK, &procmask, NULL);

        if(sem_down(han->free) == -1)
            return -1;
        sem_down(han->i_guard);

        num = randint(200000);
        han->buf[i] = num;
        gettimeofday(&te, NULL);

        i = han->i;
        han->i = (han->i + 1) % BUF_SIZE;
        in_stock = atomic_fetch_add(&han->in_stock, 1) + 1;

        printf("(%ld - %lld) Dodałem liczbę: %d na pozycji %d. Liczba zadań oczekujących: %d.\n", (long)getpid(), te.tv_sec*1000LL + te.tv_usec/1000, num, i, in_stock);

        sem_up(han->i_guard);
        sem_up(han->full);

        sigprocmask(SIG_SETMASK, &oprocmask, NULL);
    }

    return 0;
}

