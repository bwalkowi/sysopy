#ifndef _SSI_H_
#define _SSI_H_

#include <stdbool.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <poll.h>


typedef struct {
    int domain;
    time_t time_limit;
    struct pollfd sock;
} ssi_client_t;

typedef struct {
    uint32_t max_connections;
    struct pollfd *fds;
    time_t time_limit;
    time_t *last_call;
} ssi_server_t;

typedef struct {
    uint16_t max_connections;
    uint16_t port;
    char *path;
    time_t time_limit;
} ssi_server_args_t;

typedef struct {
    bool local;
    time_t time_limit;
    union {
        struct {
            char *path;
        } local;
        struct {
            uint16_t port;
            uint32_t ip;
        } remote;
    } domain;
} ssi_client_args_t;


int ssi_server_init(ssi_server_t *, ssi_server_args_t *);
int ssi_server_send(ssi_server_t *, int, void *, size_t);
int ssi_server_broadcast(ssi_server_t *, int, void *, size_t);
int ssi_server_recv(ssi_server_t *, void *, size_t);
int ssi_server_recvfrom(ssi_server_t *, int, void *, size_t);
int ssi_server_poll(ssi_server_t *, int);
int ssi_server_ready_clients(ssi_server_t *);
void ssi_server_list_clients(ssi_server_t *);
void ssi_server_shutdown(ssi_server_t *);

int ssi_client_init(ssi_client_t *, ssi_client_args_t *);
int ssi_client_send(ssi_client_t *, void *, size_t);
int ssi_client_recv(ssi_client_t *, void *, size_t);
int ssi_client_poll(ssi_client_t *);
void ssi_client_shutdown(ssi_client_t *client);

#endif // _SSI_H_

