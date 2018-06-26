#define _XOPEN_SOURCE 500

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "sig_env.h"

#include <sys/time.h>
#include <sys/resource.h>


int main(int argc, char **argv){
	if(argc != 2){
		printf("Incorrect arguments. Required ones: prog n str\n"
               "\tprog - program name\n"
               "\tn - number of signals to send\n");
		return -1;
	}

    errno = 0;
    long n = strtol(*(argv + 1), NULL, 10);
    if(errno){
        perror("offset must be a nonegative number");
        return -1;
    }

    struct rlimit lim;
    lim.rlim_cur = 100000;
    lim.rlim_max = 100000;
    setrlimit(RLIMIT_SIGPENDING, &lim);

    char *err_msg;
    if((err_msg = set_sig_han(RTSIG1, RTSIG2))){
        perror(err_msg);
        return -1;
    }

    pid_t catcher;
    if((catcher = vfork()) < 0){
        perror("vfork");
        return -1;
    }
    else if(catcher == 0){
        execl("./catcher", "catcher", NULL);
        perror("exec");
        return -1;
    }

    while(receive);
    receive = true;

    union sigval val;
    
    for(long i = 0; i < n; ++i)
        sigqueue(catcher, RTSIG1, val);

    sigqueue(catcher, RTSIG2, val);

    while(receive);

    int status;
    if(wait(&status) == -1){
        perror("wait");
        return -1;
    }

    printf("\nReceived %ld signals from %ld send\n", sig_num, n);   
	return 0;
}

