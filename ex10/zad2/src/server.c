#define _XOPEN_SOURCE 500

#include <stdbool.h>
#include <stdio.h>
#include <signal.h>

#include <arpa/inet.h>

#include "ssi.h"


bool alive;

void sigint_handler(int num){
    alive = false;
}

int main(int argc, char **argv){
    if(argc != 3){
        printf("Incorrect arguments. Required ones: path port\n"
               "\tpath - for local socket\n"
               "\tport - for remote socket\n");
        return -1;
    }

    /* block every signal except SIGINT and set SIGINT handler */
    int err = 0;
    sigset_t set;
    if(sigfillset(&set) == -1 || sigdelset(&set, SIGINT) == -1){
        perror("creating sigmask");
        return -1;
    }
    if( (err = pthread_sigmask(SIG_SETMASK, &set, NULL)) != 0){
        printf("setting sigmask: %s\n", strerror(err));
        return -1;
    }

    struct sigaction act;
    (void)memset(&act, 0, sizeof(act));
    act.sa_handler = sigint_handler;
    if(sigaction(SIGINT, &act, NULL) == -1)
        perror("setting sigint handler");

    /* create server */
    server_args_t args = {.max_connections = 32, .time_limit = 30, .path = argv[1], .port = htons((uint16_t)strtol(argv[2], NULL, 10))};
    ssi_t *server = ssi_open(true, &args);
    if(!server){
        perror("server init");
        return -1;
    }

    msg_t msg;
    alive = true;

    while(alive)
        if(ssi_wait(server) > 0)
            ssi_recv(server, &msg, sizeof(msg));

    ssi_close(server);
    return 0;
}

