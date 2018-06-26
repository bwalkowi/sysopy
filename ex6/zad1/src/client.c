#define _XOPEN_SOURCE

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
#include <unistd.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include "msg.h"

static int client;
static int server;

static void clean(){
    printf("client closing itself and connection to server\n");
    msgctl(client, IPC_RMID, NULL);
    printf("done\n");
}

void sigint_handler(int p) {
    msg_t msg;
    msg.from = getpid();
    msg.request = CLIENT_FINISHED;

    printf("\nclient sending shutdown info to server\n");
    if (msgsnd(server, (char*)&msg, sizeof(msg_t), 0) < 0){
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
    if(argc != 3){
        printf("Incorrect arguments. Required ones: prog n path char\n"
                   "\tprog - program name\n"
                   "\tpath - string used to create server\n"
                   "\tchar - ASCII character\n");
		return -1;
	}

    key_t key = ftok(argv[1], argv[2][0]);
    if(key == -1){
        perror("gen server key");
        return -1;
    }

    server = msgget(key, IPC_CREAT | S_IRWXU | S_IRWXG | S_IRWXO);
    if(server == -1){
        perror("creating server mq");
        return -1;
    }

    client = msgget(IPC_PRIVATE, IPC_CREAT | S_IRWXU | S_IRWXG | S_IRWXO);
    if(server == -1){
        perror("creating client mq");
        return -1;
    }

    msg_t msg;
    size_t msg_len = sizeof(msg);

    signal(SIGINT, sigint_handler);
    atexit(clean);

    msg.num = client;
    msg.from = getpid();
    msg.request = CLIENT_READY;
    msgsnd(server, &msg, msg_len, 0);

    while(true){
        if(msgrcv(client, &msg, msg_len, 0, 0) < 0){
            perror("client failed to receive msg"); 
            continue;
        }

        msg.from = getpid();
        msg.request = is_prime(msg.num) ? PRIME : NOT_PRIME;
        msgsnd(server, &msg, msg_len, 0);
    }

    return 0;
}
