#define _XOPEN_SOURCE 500

#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <errno.h>

#include "ssi.h"

#define SUCCES (void *)0
#define FAILURE (void *)-1


typedef struct {
    char *name;
    ssi_t *client;
} user_listener_t;

void *listen_user(void *args){
    user_listener_t *listener = (user_listener_t *)args;
    msg_t msg;
    strncpy(msg.user_name, listener->name, NAME_LEN - 1);
    msg.user_name[NAME_LEN - 1] = '\0';
    msg.msg[MSG_LEN - 1] = '\0';

    while(true){
        (void)memset(msg.msg, 0, MSG_LEN - 1);
        if(read(STDIN_FILENO, msg.msg, MSG_LEN - 1) != -1){
            if(ssi_send(listener->client, listener->client->specs.client.sock.fd, &msg, sizeof(msg)) == -1)
                perror("user input lost");
        }
        else
            perror("reading user input");
    }
    return SUCCES;
}

void *listen_server(void *args){
    ssi_t *client = (ssi_t *)args;
    msg_t msg;

    int err = 0;
    while(true){
        if((err = ssi_wait(client)) > 0){
            (void)memset(msg.msg, 0, MSG_LEN - 1);
            err = ssi_recv(client, &msg, sizeof(msg));
            if(err == -1)
                perror("reading server input");
            else if(err == 0){
                printf("server disconnected\n");
                kill(getpid(), SIGINT);
                break;
            }
            else
                printf("%s:\n%s\n", msg.user_name, msg.msg);
        }

        else{
            // printf("%d\n", err);
            kill(getpid(), SIGINT);
            break;
        }
    }
    return SUCCES;
}

int main(int argc, char **argv){
    client_args_t args = {.time_limit = 30};

    /* erad args */
    if(argc == 4){
        if(memcmp(argv[2], "local", 6) == 0)
            args.local = true;
        else{
            printf("Incorrect arguments\n");
            return -1;
        }
        args.domain.local.path = argv[3];
    }

    else if(argc == 5){
        if(memcmp(argv[2], "remote", 7) == 0)
            args.local = false;
        else{
            printf("Incorrect arguments\n");
            return -1;
        }
        args.domain.remote.port = htons((uint16_t)strtol(argv[4], NULL, 10));
        struct in_addr addr;
        if(inet_pton(AF_INET, argv[3], &addr) == -1){
            perror("incorrect ip addres");
            return -1;
        }
        args.domain.remote.ip = addr.s_addr;
    }

    else{
        printf("Incorrect arguments.\n");
        return -1;
    }

    /* block signals */
    int err = 0;
    sigset_t set;
    if(sigfillset(&set) == -1){
        perror("creating sigmask");
        return -1;
    }
    if( (err = pthread_sigmask(SIG_SETMASK, &set, NULL)) != 0){
        printf("setting sigmask: %s\n", strerror(err));
        return -1;
    }

    /* create client */
    ssi_t *client = ssi_open(false, &args);
    if(!client){
        perror("client init");
        return -1;
    }

    /* start threads */
    pthread_t  listener1, listener2;

    if((err = pthread_create(&listener1, NULL, listen_user, &(user_listener_t){.name = argv[1], .client = client}))){
        printf("creating thread failed: %s\n", strerror(err));
        goto cleanup;
    }
    if((err = pthread_create(&listener2, NULL, listen_server, client))){
        printf("creating thread failed: %s\n", strerror(err));
        goto cleanup;
    }

    int sig;
    (void)sigwait(&set, &sig);

    cleanup:
        (void)pthread_cancel(listener1);
        (void)pthread_join(listener1, NULL);
        (void)pthread_cancel(listener2);
        (void)pthread_join(listener2, NULL);
        ssi_close(client);

        return err;
}

