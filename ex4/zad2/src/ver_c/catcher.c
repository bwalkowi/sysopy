#define _XOPEN_SOURCE 500

#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <stdbool.h>
#include "sig_env.h"

#include <sys/time.h>
#include <sys/resource.h>

int main(int argc, char **argv){
    char *err_msg;
    if((err_msg = set_sig_han(RTSIG1, RTSIG2))){
        perror(err_msg);
        return -1;
    }

    struct rlimit lim;
    lim.rlim_cur = 100000;
    lim.rlim_max = 100000;
    setrlimit(RLIMIT_SIGPENDING, &lim);

    pid_t sender = getppid();

    union sigval val;

    sigqueue(sender, RTSIG2, val);

    while(receive);

    for(long i = 0; i < sig_num; ++i)
        sigqueue(sender, RTSIG1, val);

    sigqueue(sender, RTSIG2, val);

	return 0;
}

