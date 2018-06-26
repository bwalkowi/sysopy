#define _XOPEN_SOURCE 500

#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <stdbool.h>
#include "sig_env.h"


int main(int argc, char **argv){
    char *err_msg;
    if((err_msg = set_sig_han(SIGUSR1, SIGUSR2))){
        perror(err_msg);
        return -1;
    }

    if((err_msg = set_sig_mask(SIGUSR1, SIGUSR2))){
        perror(err_msg);
        return -1;
    }

    pid_t sender = getppid();

    kill(sender, SIGUSR1);

    while(receive){
        sigsuspend(&usr_unblk);
        kill(sender, SIGUSR1);
    }

    long n = sig_num;

    sigsuspend(&usr_unblk);

    for(long i = 0; i < n; ++i){
        kill(sender, SIGUSR1);
        sigsuspend(&usr_unblk);
    }

    kill(sender, SIGUSR2);
    sigsuspend(&usr_unblk);

	return 0;
}

