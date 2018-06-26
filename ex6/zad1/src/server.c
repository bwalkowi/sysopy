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

#include <fcntl.h>
#include <sys/stat.h> 
#include <unistd.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include "msg.h"

#define CNUM 20

static client_t clients[CNUM];
static int server;

static void clean(){
    printf("server closing itself\n");
    msgctl(server, IPC_RMID, NULL);
    printf("done\n");
}

static void sigint_handler(int p) {
    exit(0);
}

static int add_cmq(pid_t cpid, int mq_id){
    int avaible = -1;
    for(int i = 0; i < CNUM; ++i){
        if(clients[i].cid == cpid)
            return clients[i].cmq;
        if(clients[i].cid)
            continue;
        else if(avaible == -1)
            avaible = i;
    }
    if(avaible == -1)
        return -1;

    clients[avaible].cmq = mq_id;    
    clients[avaible].cid = cpid;
    return clients[avaible].cmq;
}

static void rm_cmq(pid_t cpid){
    for(int i = 0; i < CNUM; ++i)
        if(clients[i].cid == cpid)
            clients[i].cid = 0;
}

static int get_cmq(pid_t cpid){
    for(int i = 0; i < CNUM; ++i){
        if(clients[i].cid == cpid)
            return clients[i].cmq;
    }
    return -1;
}

int randint(int limit){
    return rand() % limit;
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
        perror("gen server key:");
        return -1;
    }

    server = msgget(key, IPC_CREAT | S_IRWXU | S_IRWXG | S_IRWXO);
    if(server == -1){
        perror("creating server mq:");
        return -1;
    }

    srand(time(NULL));

    signal(SIGINT, sigint_handler);
    atexit(clean);

    msg_t msg;
    size_t msg_len = sizeof(msg);

    int client;
    while(true){
        if(msgrcv(server, &msg, msg_len, 0, 0) < 0){
            perror("server failed to receive msg"); 
            continue;
        }

        switch(msg.request){
            case CLIENT_READY:
                client = add_cmq(msg.from, msg.num);
                if(client == -1)
                    printf("Could not make connection to client %ld\n", (long)msg.from);
                else{
                    msg.from = getpid();
                    msg.request = CHECK_IF_PRIME;
                    msg.num = randint(200000);
                    msgsnd(client, &msg, msg_len, 0);
                }
                break;

            case CLIENT_FINISHED:
                rm_cmq(msg.from);
                break;

            case PRIME:
            case NOT_PRIME:
                client = get_cmq(msg.from);
                if(client == -1)
                    printf("There is no such client %ld\n", (long)msg.from);
                else{
                    printf("Client %ld says that %d is %s\n", (long)msg.from, msg.num, msg.request == PRIME ? "prime" : "not prime");
                    msg.from = getpid();
                    msg.request = CHECK_IF_PRIME;
                    msg.num = randint(200000);
                    msgsnd(client, (char*)&msg, msg_len, 0);
                }        
                break;
        }
    }

    return 0;
}
