#ifndef _SSI_H_
#define _SSI_H_

#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>

#include <time.h>
#include <poll.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <unistd.h>


#define NAME_LEN 32
#define MSG_LEN 256

typedef struct {
    char user_name[NAME_LEN];
    char msg[MSG_LEN];
} msg_t;

typedef struct {
    bool server;
    time_t time_limit;

    union {
        struct {
            int domain;
            struct pollfd sock;
        } client;
        struct {
            uint16_t max_connections;
            struct pollfd sock[2];
            struct pollfd *fds;
            time_t *last_call;
        } server;
    } specs;    

} ssi_t;

typedef struct {
    uint16_t max_connections;
    uint16_t port;
    char *path;
    time_t time_limit;
} server_args_t;

typedef struct {
    bool local;
    time_t time_limit;
    union {
        struct {
            char *path;
        } local;
        struct {
            uint32_t ip;
            uint16_t port;
        } remote;
    } domain;
} client_args_t;

ssi_t *ssi_open(bool, void *);
void ssi_close(ssi_t *);
int ssi_ssi_broadcast(ssi_t *, void *, size_t);
int ssi_send(ssi_t *, int, void *, size_t);
int ssi_wait(ssi_t *);
int ssi_recv(ssi_t *, void *, size_t);
int ssi_recvfrom(ssi_t *, int, void *, size_t);

#endif // _SSI_H_

