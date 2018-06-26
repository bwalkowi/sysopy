#include <string.h>
#include <stdio.h>
#include <time.h>
#include <stdint.h>
#include <stdbool.h>
#include <limits.h>
#include <errno.h>
#include <stdlib.h>
#include <signal.h>
#include <math.h>

#include <fcntl.h>
#include <sys/stat.h> 
#include <mqueue.h>
#include <sys/types.h>
#include <unistd.h>

#include "msg.h"

static char cname[30];
static mqd_t client;
static mqd_t server;

static void clean(){
    printf("client closing itself and connection to server\n");
    mq_close(client);
    mq_close(server);
    mq_unlink(cname);
    printf("done\n");
}

void sigint_handler(int p) {
    msg_t msg;
    msg.from = getpid();
    msg.request = CLIENT_FINISHED;

    printf("\nclient sending shutdown info to server\n");
    if (mq_send(server, (char*)&msg, sizeof(msg_t), 0) < 0){
        perror("mq_send error");
        exit(errno);
    }
    exit(0);
}

bool is_prime(unsigned int num){
    if(num == 2 || num == 3)
        return true;
    if(num < 5)
        return false;
    if(num % 2 == 0 || num % 3 == 0)
        return false;
    for(int i = 5; i <= (int)sqrt(num); i += 6){
        if(num % i == 0 || (num + 2) % i == 0)
            return false;
    }
    return true;
}

int main(int argc, char **argv){
    if(argc != 2){
        printf("Incorrect arguments. Required ones: prog n str\n"
                   "\tprog - program name\n"
                   "\tpath - string used to create server\n");
		return -1;
	}

    snprintf(cname, 30, "%s%ld", "/client", (long)getpid());

    msg_t msg;
    size_t msg_len = sizeof(msg);

    struct mq_attr attr = {0};
    attr.mq_maxmsg = 5;
    attr.mq_msgsize = msg_len;

    mq_unlink(cname);
    client = mq_open(cname, O_CREAT | O_RDONLY, S_IRWXU | S_IRWXG | S_IRWXO, &attr);
    if(client == (mqd_t)-1){
        perror("creating client");
        return -1;
    }

    if((server = mq_open(argv[1], O_WRONLY)) == (mqd_t) -1){
        perror("creating conneciton to server");
        return -1;
    }

    signal(SIGINT, sigint_handler);
    atexit(clean);

    msg.from = getpid();
    msg.request = CLIENT_READY;
    mq_send(server, (char*)&msg, msg_len, 1);

    while(true){
        if(mq_receive(client, (char*)&msg, msg_len, NULL) < 0){
            perror("server failed to receive msg"); 
            continue;
        }

        msg.from = getpid();
        msg.request = is_prime(msg.num) ? PRIME : NOT_PRIME;
        mq_send(server, (char*)&msg, msg_len, 0);
    }

    return 0;
}
