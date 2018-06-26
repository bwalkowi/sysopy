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


int main(int argc, char **argv){
	if(argc != 2){
		printf("Incorrect arguments. Required ones: prog n\n"
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

    char *err_msg;
    if((err_msg = set_sig_han(SIGUSR1, SIGUSR2))){
        perror(err_msg);
        return -1;
    }

    if((err_msg = set_sig_mask(SIGUSR1, SIGUSR2))){
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

    sigsuspend(&usr_unblk);
    
    for(long i = 0; i < n; ++i){
        kill(catcher, SIGUSR1);
        sigsuspend(&usr_unblk);
    }

    kill(catcher, SIGUSR2);
    sigsuspend(&usr_unblk);
    kill(catcher, SIGUSR2);

    sig_num = 0;
    while(receive){
        sigsuspend(&usr_unblk);
        kill(catcher, SIGUSR1);
    }

    int status;
    if(wait(&status) == -1){
        perror("wait");
        return -1;
    }

    printf("\nReceived %ld signals from %ld send", sig_num, n);   
	return 0;
}

