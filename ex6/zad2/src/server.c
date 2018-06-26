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
#include <mqueue.h>
#include <sys/types.h>
#include <unistd.h>

#include "msg.h"

#define CNUM 20

static client_t clients[CNUM];
static char *sname;
static mqd_t server;

static void clean(){
    printf("\nserver closing clients\n");
    for(int i = 0; i < CNUM; ++i)
        if(clients[i].cid && mq_close(clients[i].cmq)){
            printf("server closing client %ld mq: %s\n", (long)clients[i].cid, strerror(errno));
        }

    printf("server closing itself\n");
    mq_close(server);
    mq_unlink(sname);
    printf("done\n");
}

static void sigint_handler(int p) {
    exit(0);
}

static mqd_t add_cmq(pid_t cpid){
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
        return (mqd_t) -1;

    char cname[30];
    snprintf(cname, 30, "%s%ld", "/client", (long)cpid);

    if((clients[avaible].cmq = mq_open(cname, O_WRONLY)) == (mqd_t) -1){
        printf("server opening client %ld mq: %s\n", (long)cpid, strerror(errno));
        return (mqd_t) -1;
    }

    clients[avaible].cid = cpid;
    return clients[avaible].cmq;
}

static void rm_cmq(pid_t cpid){
    for(int i = 0; i < CNUM; ++i){
        if(clients[i].cid == cpid){
            clients[i].cid = 0;
            if(mq_close(clients[i].cmq))
                printf("server closing client %ld mq: %s\n", (long)cpid, strerror(errno));
        }
    }
}

static mqd_t get_cmq(pid_t cpid){
    for(int i = 0; i < CNUM; ++i){
        if(clients[i].cid == cpid)
            return clients[i].cmq;
    }

    return (mqd_t) -1;
}

int randint(int limit){
    return rand() % limit;
}

int main(int argc, char **argv){
    if(argc != 2){
        printf("Incorrect arguments. Required ones: prog n path\n"
                   "\tprog - program name\n"
                   "\tpath - string used to create server\n");
		return -1;
	}

    sname = malloc((strlen(argv[1]) + 1) * sizeof(*sname));
    if(!sname){
        perror("creating server name");
        return -1;
    }
    memcpy(sname, argv[1], strlen(argv[1]) + 1);

    srand(time(NULL));

    msg_t msg;
    size_t msg_len = sizeof(msg);

    struct mq_attr attr = {0};
    attr.mq_maxmsg = 5;
    attr.mq_msgsize = msg_len;
    mq_unlink(sname);
    server = mq_open(sname, O_CREAT | O_RDONLY, S_IRWXU | S_IRWXG | S_IRWXO, &attr);
    if(server == (mqd_t)-1){
        perror("creating server");
        return -1;
    }

    signal(SIGINT, sigint_handler);
    atexit(clean);

    mqd_t client;
    while(true){
        if(mq_receive(server, (char*)&msg, msg_len, NULL) < 0){
            perror("server failed to receive msg"); 
            continue;
        }

        switch(msg.request){
            case CLIENT_READY:
                client = add_cmq(msg.from);
                if(client == (mqd_t) -1)
                    printf("Could not make connection to client %ld\n", (long)msg.from);
                else{
                    msg.from = getpid();
                    msg.request = CHECK_IF_PRIME;
                    msg.num = randint(200000);
                    mq_send(client, (char*)&msg, msg_len, 0);
                }
                break;

            case CLIENT_FINISHED:
                rm_cmq(msg.from);
                break;

            case PRIME:
            case NOT_PRIME:
                client = get_cmq(msg.from);
                if(client == (mqd_t) -1)
                    printf("There is no such client %ld\n", (long)msg.from);
                else{
                    printf("Client %ld says that %d is %s\n", (long)msg.from, msg.num, msg.request == PRIME ? "prime" : "not prime");
                    msg.from = getpid();
                    msg.request = CHECK_IF_PRIME;
                    msg.num = randint(200000);
                    mq_send(client, (char*)&msg, msg_len, 0);
                }        
                break;
        }
    }

    return 0;
}
