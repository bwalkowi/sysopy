#include "msg.h"
#include "shell.h"
#include "ssi.h"


static void run_subshell(ssi_client_t *client, shell_t *shell, int fd){
    msg_t msg;
    pid_t pid;
    int err = 0;
    TOKEN term = T_NL;
    while(shell->alive){
        if((err = ssi_client_poll(client)) > 0){
            (void)memset(msg.msg, 0, MSG_LEN);
            err = ssi_client_recv(client, msg.msg, MSG_LEN);
            if(err == -1)
                perror("reading server input");
            else if(err == 0)
                shell->alive = false;
            else{
                if(ftruncate(fd, 0) == -1){
                    fprintf(stderr, "ftruncate\n");
                    shell->alive = false;
                }
                lseek(fd, 0, SEEK_SET);
                do{
                    term = command(msg.msg, MSG_LEN, &pid, false, NULL, shell);
                    if(term == T_ERROR) {
                        fprintf(stderr, "Bad command\n");
                        term = T_NL;
                    }
                    if(term != T_AMP && pid > 0)
                        wait_and_display(pid);
                } while(term != T_NL);

                lseek(fd, 0, SEEK_SET);
                (void)memset(msg.msg, 0, MSG_LEN);
                msg.request = START;
                int nread = 0;
                while( (nread = read(fd, msg.msg, MSG_LEN - 1)) > 0){
                    msg.msg[nread] = '\0';
                    ssi_client_send(client, &msg, sizeof(msg));
                }
                msg.msg[0] = '\0';
                msg.request = STOP;
                ssi_client_send(client, &msg, sizeof(msg));
            }
        }

        else
            shell->alive = false;
    }
}

static int read_args(int argc, char **argv, ssi_client_args_t *args){
    if(argc == 3){
        if(memcmp(argv[1], "local", 6) == 0)
            args->local = true;
        else{
            printf("Incorrect arguments\n");
            return -1;
        }
        args->domain.local.path = argv[2];
        return 0;
    }

    else if(argc == 4){
        if(memcmp(argv[1], "remote", 7) == 0)
            args->local = false;
        else{
            printf("Incorrect arguments\n");
            return -1;
        }
        args->domain.remote.port = htons((uint16_t)strtol(argv[3], NULL, 10));
        struct in_addr addr;
        if(inet_pton(AF_INET, argv[2], &addr) == -1){
            perror("incorrect ip addres");
            return -1;
        }
        args->domain.remote.ip = addr.s_addr;
        return 0;
    }

    else{
       printf("Incorrect arguments.\n");
       return -1;
    }
}

int main(int argc, char **argv){
    ssi_client_args_t args = {.time_limit = -1};
    if(read_args(argc, argv, &args) == -1)
        return -1;

    pid_t pid;
    if( (pid = fork()) == -1 )
        printf("sth went wrong\n");
    else if(pid == 0){
        /* create tmp file */
        char tmp_file[32];
        memset(tmp_file, 0, sizeof(tmp_file));
        strncpy(tmp_file,"/tmp/myTmpFile-XXXXXX", 21);
        int fd = mkstemp(tmp_file);
        if(fd == -1){
            perror("mk tmp file");
            return -1;
        }
        unlink(tmp_file);

        /* init client */
        shell_t shell;
        if(shell_init(&shell) == -1){
            perror("subshell init");
            return -1;
        }

        /* init subshell */
        ssi_client_t client;
        if(ssi_client_init(&client, &args) == -1){
            perror("client init");
            shutdown_shell(&shell);
            ssi_client_shutdown(&client);
            return -1;
        }            

        /* init separate session */
        if(setsid() == -1){
            perror("setsid");
            shutdown_shell(&shell);
            ssi_client_shutdown(&client);
            return -1;
        }

        /* close/redirect std streams */
        if(close(STDIN_FILENO) == -1 || dup2(fd, STDOUT_FILENO) == -1 || dup2(fd, STDERR_FILENO) == -1){
            perror("closing stdstrems");
            shutdown_shell(&shell);
            ssi_client_shutdown(&client);
            return -1;
        }

        /* run subshell */
        run_subshell(&client, &shell, fd);
        ssi_client_shutdown(&client);
        shutdown_shell(&shell);
    }

    return 0;
}

