#include "ssi.h"


static int addr_un_init(struct sockaddr_un *addr, char *path){
    if(!addr || !path){
        errno = EINVAL;
        return -1;
    }

    if(sizeof(struct sockaddr_un) - sizeof(sa_family_t) < strlen(path) - 1){
        errno = ENAMETOOLONG;
        return -1;
    }

    (void)memset(addr, 0, sizeof(*addr));
    addr->sun_family = AF_UNIX;
    strcpy(addr->sun_path, path);
    return 0;
}
    
static int addr_in_init(struct sockaddr_in *addr, uint32_t ip, uint16_t port){
    (void)memset(addr, 0, sizeof(*addr));
    addr->sin_family = AF_INET;
    addr->sin_addr.s_addr = ip;
    addr->sin_port = port;
    return 0;
}

static inline void close_socket(int socket_fd){
    (void)shutdown(socket_fd, SHUT_RDWR);
    (void)close(socket_fd);
}

static void close_sockets(struct pollfd *fds, uint16_t size){
    for(uint16_t i = 0; i < size; ++i)
        close_socket(fds[i].fd);
}

inline static int server_resource_init(ssi_t *tmp, uint16_t size){
    if(!(tmp->specs.server.last_call = calloc(size, sizeof(*tmp->specs.server.last_call))))
        return -1;

    if(!(tmp->specs.server.fds = calloc(size, sizeof(*tmp->specs.server.fds)))){
        free(tmp->specs.server.last_call);
        return -1;
    }
    for(uint16_t i = 0; i < size; ++i){
        tmp->specs.server.fds[i].fd = -1;
        tmp->specs.server.fds[i].events = POLLIN | POLLHUP;
    }

    return 0;
}

static int server_sock_init(ssi_t *tmp, server_args_t *args){
    struct sockaddr_storage addr;

    tmp->specs.server.sock[0].fd = -1;
    tmp->specs.server.sock[0].events = POLLIN;

    tmp->specs.server.sock[1].fd = -1;
    tmp->specs.server.sock[1].events = POLLIN;

    if(addr_un_init((struct sockaddr_un*)&addr, args->path) == -1 || (tmp->specs.server.sock[0].fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
        return -1;

    if(bind(tmp->specs.server.sock[0].fd, (struct sockaddr*)&addr, sizeof(struct sockaddr_un)) == -1)
        goto cleanup;

    addr_in_init((struct sockaddr_in *)&addr, INADDR_ANY, args->port);
    if((tmp->specs.server.sock[1].fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        goto cleanup;

    if(bind(tmp->specs.server.sock[1].fd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) == -1)
        goto cleanup;

    if(listen(tmp->specs.server.sock[0].fd, args->max_connections / 2) == -1 || listen(tmp->specs.server.sock[1].fd, args->max_connections / 2) == -1)
        goto cleanup;

    return 0;

    cleanup:
        close_socket(tmp->specs.server.sock[0].fd);
        close_socket(tmp->specs.server.sock[1].fd);
        return -1;
}

static int server_init(ssi_t *server, server_args_t *args){
    server->server = true;
    server->time_limit = args->time_limit;
    server->specs.server.max_connections = args->max_connections;

    if(server_resource_init(server, args->max_connections) == -1 || server_sock_init(server, args) == -1)
        return -1;

    return 0;
}

static int client_init(ssi_t *client, client_args_t *args){
    client->server = false;
    client->time_limit = args->time_limit;
    client->specs.client.sock.events = POLLIN;

    struct sockaddr_storage addr;
    size_t addr_len = 0;

    if(args->local){
        client->specs.client.domain = AF_UNIX;
        addr_len = sizeof(struct sockaddr_un);
        if(addr_un_init((struct sockaddr_un *)&addr, args->domain.local.path) == -1)
            return -1;
    }
    else{
        client->specs.client.domain = AF_INET;
        addr_len = sizeof(struct sockaddr_in);
        if(addr_in_init((struct sockaddr_in *)&addr, args->domain.remote.ip, args->domain.remote.port) == -1)
            return -1;
    }

    if((client->specs.client.sock.fd = socket(client->specs.client.domain, SOCK_STREAM, 0)) == -1 || connect(client->specs.client.sock.fd, (struct sockaddr *)&addr, addr_len) == -1){
        close_socket(client->specs.client.sock.fd);
        return -1;
    }

    return 0;
}

ssi_t *ssi_open(bool server, void *args){
    if(!args){
        errno = EINVAL;
        return NULL;
    }

    ssi_t *tmp = calloc(1, sizeof(*tmp));
    if(!tmp)
        return NULL;

    if(server ? server_init(tmp, args) : client_init(tmp, args)){
        free(tmp);
        return NULL;
    }
    else
        return tmp;
}

void ssi_close(ssi_t *tmp){
    if(tmp->server){
        close_sockets(tmp->specs.server.fds, tmp->specs.server.max_connections);
        close_socket(tmp->specs.server.sock[0].fd);
        close_socket(tmp->specs.server.sock[1].fd);

        free(tmp->specs.server.fds);
        free(tmp->specs.server.last_call);
    }
    else
        close_socket(tmp->specs.client.sock.fd);

    free(tmp);
}

int ssi_broadcast(ssi_t *tmp, int socket_fd, void *msg, size_t msg_len){
    int err = 0;
    if(tmp->server){
        for(uint16_t i = 0; i < tmp->specs.server.max_connections; ++i)
            if(tmp->specs.server.fds[i].fd != -1 && tmp->specs.server.fds[i].fd != socket_fd){
                err |= send(tmp->specs.server.fds[i].fd, msg, msg_len, 0);
                if(err == -1)
                    perror("server send");
            }
    }
    else
        err = send(tmp->specs.client.sock.fd, msg, msg_len, 0);

    return err;
}

int ssi_send(ssi_t *tmp, int socket_fd, void *msg, size_t msg_len){
    int err = 0;
    bool found = false;
    if(tmp->server){
        for(uint16_t i = 0; i < tmp->specs.server.max_connections; ++i)
            if(tmp->specs.server.fds[i].fd == socket_fd){
                err = send(socket_fd, msg, msg_len, 0);
                found = true;
                break;
            }
        if(!found){
            errno = EINVAL;
            err = -1;
        }
    }

    else if(tmp->specs.client.sock.fd == socket_fd){
        err = send(socket_fd, msg, msg_len, 0);
    }

    else{
        errno = EINVAL;
        err = -1;
    }

    return err;
}

int ssi_recv(ssi_t *tmp, void *msg, size_t msg_len){
    int err = 0;

    if(tmp->server){
        for(uint16_t i = 0; i < tmp->specs.server.max_connections; ++i)
            if(tmp->specs.server.fds[i].revents == POLLIN){
                time(&tmp->specs.server.last_call[i]);
                err = recv(tmp->specs.server.fds[i].fd, msg, msg_len, 0);
                if(err > 0)
                    ssi_broadcast(tmp, tmp->specs.server.fds[i].fd, msg, msg_len);

                else if(err == 0){
                    close_socket(tmp->specs.server.fds[i].fd);
                    tmp->specs.server.fds[i].fd = -1;
                    tmp->specs.server.last_call[i] = 0;
                    printf("client disconnected\n");
                }
           }
    }
    else
        err = recv(tmp->specs.client.sock.fd, msg, msg_len, 0);

    return err;
}

int ssi_recvfrom(ssi_t *tmp, int socket_fd, void *msg, size_t msg_len){
    int err = 0;
    bool found = false;
    if(tmp->server){
        for(uint16_t i = 0; i < tmp->specs.server.max_connections; ++i)
            if(tmp->specs.server.fds[i].fd == socket_fd){
                err = recv(socket_fd, msg, msg_len, 0);
                time(&tmp->specs.server.last_call[i]);
                found = true;
                break;
            }
        if(!found){
            errno = EINVAL;
            err = -1;
        }
    }

    else if(tmp->specs.client.sock.fd == socket_fd)
        err = recv(socket_fd, msg, msg_len, 0);

    else{
        errno = EINVAL;
        err = -1;
    }

    return err;
}

static void accept_connections(ssi_t *server){
    int con = poll(&server->specs.server.sock[0], 1, 0);
    if(con == 1){
        int socket_fd = accept(server->specs.server.sock[0].fd, NULL, 0);
        if(socket_fd != -1){
            for(uint16_t i = 0; i < server->specs.server.max_connections; ++i)
                if(server->specs.server.last_call[i] == 0){
                    time(&server->specs.server.last_call[i]);
                    server->specs.server.fds[i].fd = socket_fd;
                    printf("client accepted\n");
                    break;
                }
        }
    }

    con = poll(&server->specs.server.sock[1], 1, 0);
    if(con == 1){
        int socket_fd = accept(server->specs.server.sock[1].fd, NULL, 0);
        if(socket_fd != -1){
            for(uint16_t i = 0; i < server->specs.server.max_connections; ++i)
                if(server->specs.server.last_call[i] == 0){
                    time(&server->specs.server.last_call[i]);
                    server->specs.server.fds[i].fd = socket_fd;
                    printf("client accepted\n");
                    break;
                }
        }
    }
}

static void check_connections(ssi_t *server){
    time_t t;
    time(&t);

    for(uint16_t i = 0; i < server->specs.server.max_connections; ++i)
        if(server->specs.server.last_call[i] != 0 && (t - server->specs.server.last_call[i]) > server->time_limit){
            printf("Connections %d not responding - shutdown\n", i);
            close_socket(server->specs.server.fds[i].fd);
            server->specs.server.last_call[i] = 0;
            server->specs.server.fds[i].fd = -1;
        }
        else if(server->specs.server.fds[i].revents & POLLHUP){
            close_socket(server->specs.server.fds[i].fd);
            server->specs.server.fds[i].fd = -1;
            server->specs.server.last_call[i] = 0;
            printf("client disconnected\n");
        }
}

int ssi_wait(ssi_t *tmp){
    int err = 0;
    if(tmp->server){
        accept_connections(tmp);
        err = poll(tmp->specs.server.fds, tmp->specs.server.max_connections, tmp->time_limit); // * 1000);
        check_connections(tmp);
    }

    else
        err = poll(&tmp->specs.client.sock, 1, tmp->time_limit * 1000);

    return err;
}

