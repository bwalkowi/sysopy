#define _GNU_SOURCE

#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include <sys/mman.h>
#include <sys/times.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <time.h>


static long counter = 0;


int fun(void *arg) {
    clock_t time = clock();
    ++counter;
    _exit(clock() - time);
}

int main(int argc, char **argv){

	if(argc != 3){
		printf("Incorrect arguments. Required ones: prog n op\n"
               "\tprog - program name\n"
               "\tn - number of iterations\n"
               "\top - option used to spawn child: fork, vfork, clone, vclone\n");
		return -1;
	}

    errno = 0;
    long n = strtol(*(argv + 1), NULL, 10);
    if(errno){
        perror("offset must be a nonegative number");
        return -1;
    }

    char *options[] = {"fork", "vfork", "clone", "vclone"};
    int op = -1;
    for(int i = 0; i < sizeof(options) / sizeof(*options); ++i)
        if(!strcmp(*(argv + 2), options[i]))
            op = i;

    if(op == -1){
        printf("Incorrect option. available ones: fork, vfork, clone, vclone\n");
        return -1;
    }

    pid_t pid         =     0;
    clock_t start     =     clock();
    clock_t ctimes    =     0;

    size_t stack_len    =       1024;
    void *stack         =       mmap(NULL, stack_len, PROT_WRITE | PROT_READ, MAP_STACK | MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    if(stack == MAP_FAILED){
        perror("mmap");
        return -1;
    }

    int status;
    for (int i = 0; i < n; ++i){
        switch (op){

            case 0:
                pid = fork();
                break;

            case 1:
                pid = vfork();
                break;

            case 2:
                pid = clone(&fun, (char*)stack + stack_len, SIGCHLD, NULL);
                break;

            case 3:
                pid = clone(&fun, (char*)stack + stack_len, SIGCHLD | CLONE_VM | CLONE_VFORK, NULL);
                break;
        }

        if(pid == -1){
            perror("spawn failed");
            return -1;
        }
        else if(pid == 0){
            fun(NULL);
        }
        else{
            wait(&status);
            if(WIFEXITED(status))
                ctimes += WEXITSTATUS(status);
        }    
    }

    munmap(stack, stack_len);

    clock_t end = clock();

    struct rusage rus;
    getrusage(RUSAGE_SELF, &rus);
    double real1      =   (double)(end - start - ctimes) / CLOCKS_PER_SEC;
    double user1     =   (double)rus.ru_utime.tv_sec + (double)rus.ru_utime.tv_usec / 10e6;
    double sys1       =   (double)rus.ru_stime.tv_sec + (double)rus.ru_stime.tv_usec / 10e6;

    printf("N: %ld\tOPTION: %s\n"
               "ORIGINAL:\n"
               "stime:\t\t%lf\n"
               "utime:\t\t%lf\n"
               "(u+s)time:\t%lf\n"
               "rtime:\t\t%lf\n", counter, options[op], sys1, user1, user1 + sys1, real1);

    getrusage(RUSAGE_CHILDREN, &rus);
    double real2      =   (double)ctimes / CLOCKS_PER_SEC;
    double user2     =   (double)rus.ru_utime.tv_sec + (double)rus.ru_utime.tv_usec / 10e6;
    double sys2       =   (double)rus.ru_stime.tv_sec + (double)rus.ru_stime.tv_usec / 10e6;

    printf("CHILDREN:\n"
               "stime:\t\t%lf\n"
               "utime:\t\t%lf\n"
               "(u+s)time:\t%lf\n"
               "rtime:\t\t%lf\n", sys2, user2, user2 + sys2, real2);

    printf("ORIGIN + CHILD:\n"
               "stime:\t\t%lf\n"
               "utime:\t\t%lf\n"
               "(u+s)time:\t%lf\n"
               "rtime:\t\t%lf\n\n", sys1 + sys2, user1 + user2, sys1 + user1 + user2 + sys2, real1 + real2);

    return 0;
}

