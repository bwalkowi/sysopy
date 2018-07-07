#include "ssi.h"


#define member_size(type, member) sizeof(((type *)0)->member)

static inline void addr_un_init(struct sockaddr_un *addr, char *path){
    (void)memset(addr, 0, sizeof(*addr));
    addr->sun_family = AF_UNIX;
    strcpy(addr->sun_path, path);
}

static inline void addr_in_init(struct sockaddr_in *addr, uint32_t ip, uint16_t port){
    (void)memset(addr, 0, sizeof(*addr));
    addr->sin_family = AF_INET;
    addr->sin_addr.s_addr = ip;
    addr->sin_port = port;
}

static inline void close_socket(int socket_fd){
    (void)shutdown(socket_fd, SHUT_RDWR);
    (void)close(socket_fd);
}

static inline void close_sockets(struct pollfd *fds, uint32_t size){
    for(uint32_t i = 0; i < size; ++i)
        close_socket(fds[i].fd);
}

int ssi_client_init(ssi_client_t *client, ssi_client_args_t *args){
    if(!client || !args){
        errno = EINVAL;
        return -1;
    }

    (void)memset(client, 0, sizeof(*client));
    client->time_limit = args->time_limit;
    client->sock.fd = -1;
    client->sock.events = POLLIN | POLLHUP;

    struct sockaddr_storage addr;
    size_t addr_len;

    if(args->local){

        if(!args->domain.local.path){
            errno = EINVAL;
            return -1;
        }
        if(strlen(args->domain.local.path) + 1 > member_size(struct sockaddr_un, sun_path)){
            errno = ENAMETOOLONG;
            return -1;
        }

        client->domain = AF_UNIX;
        addr_len = sizeof(struct sockaddr_un);
        addr_un_init((struct sockaddr_un *)&addr, args->domain.local.path);
    }
    else{
        client->domain = AF_INET;
        addr_len = sizeof(struct sockaddr_in);
        addr_in_init((struct sockaddr_in *)&addr, args->domain.remote.ip, args->domain.remote.port);
    }

    if((client->sock.fd = socket(client->domain, SOCK_STREAM, 0)) == -1 || connect(client->sock.fd, (struct sockaddr *)&addr, addr_len) == -1){
        close_socket(client->sock.fd);
        return -1;
    }

    return 0;
}

void ssi_client_shutdown(ssi_client_t *client){
    if(client)
        close_socket(client->sock.fd);
}

static int server_sock_init(ssi_server_t *server, ssi_server_args_t *args){
    struct sockaddr_storage addr;

    addr_un_init((struct sockaddr_un*)&addr, args->path);
    if( (server->fds[0].fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
        return -1;
    if(bind(server->fds[0].fd, (struct sockaddr*)&addr, sizeof(struct sockaddr_un)) == -1)
        return -1;

    addr_in_init((struct sockaddr_in *)&addr, INADDR_ANY, args->port);
    if( (server->fds[1].fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        return -1;
    if(bind(server->fds[1].fd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) == -1)
        return -1;

    if(listen(server->fds[0].fd, args->max_connections / 2) == -1 || listen(server->fds[1].fd, args->max_connections / 2) == -1)
        return -1;

    return 0;
}

int ssi_server_init(ssi_server_t *server, ssi_server_args_t *args){
    if(!server || !args){
        errno = EINVAL;
        return -1;
    }

    if(strlen(args->path) + 1 > member_size(struct sockaddr_un, sun_path)){
        errno = ENAMETOOLONG;
        return -1;
    }

    (void)memset(server, 0, sizeof(*server));
    server->time_limit = args->time_limit;
    server->max_connections = 2 + args->max_connections;

    if(!(server->fds = calloc(server->max_connections, sizeof(*server->fds))))
        return -1;

    for(uint32_t i = 0; i < server->max_connections; ++i){
        server->fds[i].fd = -1;
        server->fds[i].events = POLLIN | POLLHUP;
    }

    if(!(server->last_call = calloc(server->max_connections, sizeof(*server->last_call))) || server_sock_init(server, args) == -1){
        ssi_server_shutdown(server);
        return -1;
    }

    return 0;
}

void ssi_server_shutdown(ssi_server_t *server){
    if(server){
        close_sockets(server->fds, server->max_connections);
        free(server->fds);
        free(server->last_call);
    }
}

int ssi_server_send(ssi_server_t *server, int socket_fd, void *msg, size_t msg_len){
    if(!server || !msg || socket_fd == -1){
        errno = EINVAL;
        return -1;
    }

    for(uint32_t i = 2; i < server->max_connections; ++i)
        if(server->fds[i].fd == socket_fd)
            return send(server->fds[i].fd, msg, msg_len, 0);

    errno = EINVAL;
    return -1;
}

int ssi_client_send(ssi_client_t *client, void *msg, size_t msg_len){
    if(!client || !msg){
        errno = EINVAL;
        return -1;
    }

    return send(client->sock.fd, msg, msg_len, 0);
}

int ssi_client_recv(ssi_client_t *client, void *msg, size_t msg_len){
    if(!client || !msg){
        errno = EINVAL;
        return -1;
    }

    return recv(client->sock.fd, msg, msg_len, 0);
}

int ssi_client_poll(ssi_client_t *client){
    if(!client){
        errno = EINVAL;
        return -1;
    }

    return poll(&client->sock, 1, client->time_limit * 1000);
}

int ssi_server_broadcast(ssi_server_t *server, int socket_fd, void *msg, size_t msg_len){
    if(!server || !msg){
        errno = EINVAL;
        return -1;
    }

    int err = 0;
    for(uint32_t i = 2; i < server->max_connections; ++i)
        if(server->fds[i].fd != -1 && server->fds[i].fd != socket_fd)
            if(send(server->fds[i].fd, msg, msg_len, 0) == -1){
                perror("server broadcasting msg");
                err = -1;
            }

    return err;
}

int ssi_server_recv(ssi_server_t *server, void *msg, size_t msg_len) {
    static int32_t i = 2;

    for (; i < server->max_connections; ++i)
        if (server->fds[i].revents == POLLIN) {
            server->last_call[i] = 0;
            int err = recv(server->fds[i].fd, msg, msg_len, 0);
            if (err > 0){
                ++i;
                return server->fds[i-1].fd;
            }
            else if (err == 0) {
                printf("client disconnected\n");
                close_socket(server->fds[i].fd);
                server->last_call[i] = 0;
                server->fds[i].fd = -1;
            }
            else{
                ++i;
                return err;
            }
        }

    i = 2;
    return 0;
}

int ssi_server_ready_clients(ssi_server_t *server){
    static int32_t i = 2;

    for (; i < server->max_connections; ++i)
        if (server->fds[i].revents == POLLIN) {
            ++i;
            return server->fds[i-1].fd;
        }

    i = 2;
    return 0;
}

int ssi_server_recvfrom(ssi_server_t *server, int client, void *msg, size_t msg_len){
    for (uint32_t i = 0; i < server->max_connections; ++i)
        if (server->fds[i].fd == client){
            int err = recv(server->fds[i].fd, msg, msg_len, 0);
            if (err > 0)
                return 0;
            else if (err == 0) {
                printf("client disconnected\n");
                close_socket(server->fds[i].fd);
                server->last_call[i] = 0;
                server->fds[i].fd = -1;
                return 0;
            }
            else
                return err;
        }

    return -1;
}

static void add_connection(ssi_server_t *server, int socket_fd, time_t t){
    for(uint32_t i = 0; i < server->max_connections; ++i)
        if(server->fds[i].fd == -1){
            server->last_call[i] = t;
            server->fds[i].fd = socket_fd;
            printf("client accepted %d\n", socket_fd);
            break;
        }
}

static void check_connections(ssi_server_t *server, int *err){
    time_t t;
    time(&t);

    if(server->fds[0].revents == POLLIN){
        --*err;
        int socket_fd = accept(server->fds[0].fd, NULL, 0);
        if(socket_fd != -1)
            add_connection(server, socket_fd, t);
    }
    if(server->fds[1].revents == POLLIN){
        --*err;
        int socket_fd = accept(server->fds[1].fd, NULL, 0);
        if(socket_fd != -1)
            add_connection(server, socket_fd, t);
    }

    for(uint32_t i = 2; i < server->max_connections; ++i)
        if(server->fds[i].revents & POLLHUP /* || (server->last_call[i] != 0 && (t - server->last_call[i]) > server->time_limit) */){
            printf("client disconnected\n");
            close_socket(server->fds[i].fd);
            server->last_call[i] = 0;
            server->fds[i].fd = -1;
        }
}

int ssi_server_poll(ssi_server_t *server, int client){
    if(!server){
        errno = EINVAL;
        return -1;
    }

    int err = 0;
    if(client == -1){
        err = poll(server->fds, server->max_connections, server->time_limit * 1000);
        if(err >= 0)
            check_connections(server, &err);
    }
    else{
        for(uint32_t i = 2; i < server->max_connections; ++i)
            if(server->fds[i].fd == client)
                err = poll(&server->fds[i], 1, server->time_limit * 1000);
    }

    return err;
}

void ssi_server_list_clients(ssi_server_t *server){
    for(uint32_t i = 2; i < server->max_connections; ++i)
        if(server->fds[i].fd != -1)
            printf("client nr: %d\n", server->fds[i].fd);
}

