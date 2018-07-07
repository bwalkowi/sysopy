#include "msg.h"
#include "shell.h"


static ssi_server_t server;
static bool inited = false;

static int server_init(int argc, char **argv){
    if(argc != 3){
        printf("incorrect arguments, required ones: path port\n"
                   "\tpath - for local socket\n"
                   "\tport - for remote socket\n");
        return -1;
    }
    if(!inited){
        ssi_server_args_t args = {.max_connections = 32, .time_limit = 0, .path = argv[1], .port = htons((uint16_t)strtol(argv[2], NULL, 10))};
        if(ssi_server_init(&server, &args) == -1){
            perror("server init");
            return -1;
        }
        inited = true;
        return 0;
    }
    else{
        fprintf(stderr, "server already inited\n");
        return -1;
    }
}

static void shutdown_server(){
    if(inited){
        ssi_server_shutdown(&server);
        inited = false;
    }
}

static void order_slave(void *msg, int fd, size_t msg_len){
    if(inited){
        if(fd == -1)
            ssi_server_broadcast(&server, -1, msg, msg_len);
        else
            ssi_server_send(&server, fd, msg, msg_len);
    }
    else
        fprintf(stderr, "server not inited\n");
}

static void interrogate_slave(int fd){
    if(inited){
        if(fd == -1){
            if(ssi_server_poll(&server, -1) > 0){
                msg_t msg;
                int client;
                while( (client = ssi_server_ready_clients(&server))){
                    printf("deamon %d:\n", client);
                    int err = 0;
                    do{
                        err = ssi_server_recvfrom(&server, client, &msg, sizeof(msg));
                        printf("%s", msg.msg);
                    }while(/*err > 0 &&*/ msg.request != STOP);
                }
            }
            else
                printf("no response\n");
        }
        else{
            if(ssi_server_poll(&server, fd) > 0){
                printf("deamon %d:\n", fd);
                int err = 0;
                msg_t msg;
                do{
                    err = ssi_server_recvfrom(&server, fd, &msg, sizeof(msg));
                    printf("%s", msg.msg);
                }while(/*err > 0 && */ msg.request != STOP);
            }
            else
                printf("no response\n");
        }
    }
    else
        fprintf(stderr, "server not inited\n");
}

static bool set_sigenv(shell_t *shell){
    static bool first = true;

    int err = 0;
    struct sigaction sa;
    (void)memset(&sa, 0, sizeof(sa));
    sa.sa_handler = SIG_IGN;

    if(first){
        first = false;
        err |= sigaction(SIGINT, &sa, &shell->sig_int);
        err |= sigaction(SIGQUIT, &sa, &shell->sig_quit);
        err |= sigaction(SIGTSTP, &sa, &shell->sig_tstp);
        err |= sigaction(SIGTTIN, &sa, &shell->sig_ttin);
        err |= sigaction(SIGTTOU, &sa, &shell->sig_ttou);
        err |= sigaction(SIGCHLD, &sa, &shell->sig_chld);
    }
    else{
        err |= sigaction(SIGINT, &sa, NULL);
        err |= sigaction(SIGQUIT, &sa, NULL);
        err |= sigaction(SIGTSTP, &sa, NULL);
        err |= sigaction(SIGTTIN, &sa, NULL);
        err |= sigaction(SIGTTOU, &sa, NULL);
        err |= sigaction(SIGCHLD, &sa, NULL);
    }

    return err == 0;
}

static bool restore_sigenv(shell_t *shell){
    int err = sigaction(SIGINT, &shell->sig_int, NULL);
    err |= sigaction(SIGQUIT, &shell->sig_quit, NULL);
    err |= sigaction(SIGTSTP, &shell->sig_tstp, NULL);
    err |= sigaction(SIGTTIN, &shell->sig_ttin, NULL);
    err |= sigaction(SIGTTOU, &shell->sig_ttou, NULL);
    err |= sigaction(SIGCHLD, &shell->sig_chld, NULL);

    return err == 0;
}

static inline void display_status(pid_t pid, int status){
    printf("Process %ld: ", (long)pid);
    if(WIFEXITED(status))
        printf("Exit value %d\n", WEXITSTATUS(status));
    else{
        if(WIFSIGNALED(status))
            printf("term by signal %d", WTERMSIG(status));
#ifdef WCOREDUMP
        if(WCOREDUMP(status))
            printf(" - core dumped");
#endif
        if(WIFSTOPPED(status))
            printf(" (stopped)");
        else if (WIFCONTINUED(status))
            printf(" (continued)");
        printf("\n");
    }
}

static bool builtin(int argc, char *argv[], int srcfd, int dstfd, shell_t *shell){
    if(strcmp(argv[0], "cd") == 0){
        char *path = NULL;
        if(argc > 1)
            path = argv[1];
        else if( (path = getenv("HOME")) == NULL)
            path = ".";
        if(chdir(path) == -1)
            fprintf(stderr, "%s: bad directory\n", path);
    }
    else if(strcmp(argv[0], "mkserv") == 0){
        server_init(argc, argv);
    }
    else if(strcmp(argv[0], "rmserv") == 0){
        if(argc != 1)
            fprintf(stderr, " too many args\n");
        shutdown_server();
    }
    else if(strcmp(argv[0], "ds") == 0){
        if(argc != 1)
            fprintf(stderr, " too many args\n");
        ssi_server_list_clients(&server);
    }
    else if(strcmp(argv[0], "hears") == 0){
        if(argc != 2){
            fprintf(stderr, " incorrect args\n");
            return true;
        }
        if(strcmp(argv[1], "all") == 0)
            interrogate_slave(-1);
        else
            interrogate_slave(atoi(argv[1]));
    }
    else if(strcmp(argv[0], "ords") == 0){
        if(argc != 2){
            fprintf(stderr, " incorrect args\n");
            return true;
        }

        msg_t msg;
        if(read(STDIN_FILENO, msg.msg, MSG_LEN) == -1)
            perror("user input lost");

        if(strcmp(argv[1], "all") == 0)
            order_slave(msg.msg, -1, MSG_LEN);
        else
            order_slave(msg.msg, atoi(argv[1]), MSG_LEN);
    }
    else if(strcmp(argv[0], "exit") == 0){
        shell->alive = false;
    }
    else
        return false;

    if (srcfd != STDIN_FILENO || dstfd != STDOUT_FILENO)
        fprintf(stderr, "Illegal redirection or pipeline\n");
    return true;
}

static void redirect(int srcfd, const char *srcfile, int dstfd, const char *dstfile, bool append, bool bg){
    static char *dev = "/dev/null";
    if(srcfd == STDIN_FILENO && bg){
        srcfile = dev;
        srcfd = -1;
    }

    if(srcfile)
    if( (srcfd = open(srcfile, O_RDONLY,  0)) == -1)
        _exit(EXIT_FAILURE);
    if(srcfd != STDIN_FILENO)
    if(dup2(srcfd, STDIN_FILENO) == -1 || close(srcfd) == -1)
        _exit(EXIT_FAILURE);

    if(dstfile){
        int flags = O_WRONLY | O_CREAT;
        if (append)
            flags |= O_APPEND;
        else
            flags |= O_TRUNC;
        if( (dstfd = open(dstfile, flags, S_IRWXU | S_IRWXG | S_IRWXO)) == -1)
            _exit(EXIT_FAILURE);
    }
    if(dstfd != STDOUT_FILENO)
    if(dup2(dstfd, STDOUT_FILENO) == -1 || close(dstfd) == -1)
        _exit(EXIT_FAILURE);

    return;
}

static pid_t invoke(int argc, char *argv[], int srcfd, const char *srcfile, int dstfd, const char *dstfile, bool append, bool bg, int closefd, shell_t *shell){
    if (argc == 0 || builtin(argc, argv, srcfd, dstfd, shell))
        return 0;

    pid_t pid;
    if( (pid = fork()) == -1){
        fprintf(stderr, "Can't create new process\n");
        return 0;
    }
    else if(pid == 0){
        if(closefd != -1)
            (void)close(closefd);

        if(!bg && !restore_sigenv(shell))
            _exit(EXIT_FAILURE);
        redirect(srcfd, srcfile, dstfd, dstfile, append, bg);

        char *cmdname = strchr(argv[0], '/');
        if (cmdname == NULL)
            cmdname = argv[0];
        else
            ++cmdname;

        char *cmdpath = argv[0];
        argv[0] = cmdname;
        execvp(cmdpath, argv);
        fprintf(stderr, "Can't execute %s\n", cmdpath);
        _exit(EXIT_FAILURE);
    }
    else{
        if (srcfd != STDIN_FILENO)
            (void)close(srcfd);
        if (dstfd != STDOUT_FILENO)
            (void)close(dstfd);
        if(bg)
            printf("%ld\n", (long)pid);
        return pid;
    }
}

static TOKEN get_token(char *command, size_t command_len, char **word, size_t *word_len){
    static size_t i = 0;
    enum {NEUTRAL, GT, INQUOTE, INWORD} state = NEUTRAL;

    int c;
    while( (c = command[i++]) != '\0' && i <= command_len){
        switch(state){
            case NEUTRAL:
                switch(c){
                    case ';':
                        return T_SEMI;
                    case '&':
                        return T_AMP;
                    case '|':
                        return T_BAR;
                    case '<':
                        return T_LT;
                    case '\n':
                        i = 0;
                        return T_NL;
                    case ' ':
                    case '\t':
                        continue;
                    case '>':
                        state = GT;
                        continue;
                    case '"':
                        *word = command + i;
                        state = INQUOTE;
                        continue;
                    default:
                        *word = command + i - 1;
                        state = INWORD;
                        continue;
                }

            case GT:
                if (c == '>')
                    return T_GTGT;
                --i;
                return T_GT;

            case INQUOTE:
                switch (c) {
                    case '"':
                        *word_len = command + i - *word - 1;
                        return T_WORD;
                    default:
                        continue;
                }

            case INWORD:
                switch (c) {
                    case ';':
                    case '&':
                    case '|':
                    case '<':
                    case '>':
                    case '\n':
                    case ' ':
                    case '\t':
                        --i;
                        *word_len = command + i - *word;
                        return T_WORD;
                    default:
                        continue;
                }
        }
    }

    i = 0;
    if(state != NEUTRAL)
        return T_ERROR;
    else
        return T_EOF;
}

TOKEN command(char *input, size_t input_len, pid_t *wpid, bool makepipe, int *pipefdp, shell_t *shell){
    TOKEN token, term;
    int pfd[2] = {-1, -1};
    int pid;

    char *srcfile = NULL;
    int srcfd = STDIN_FILENO;
    char *dstfile = NULL;
    int dstfd = STDOUT_FILENO;
    bool append = false;

    int argc = 0;
    char *argv[MAXARG];

    char *word = NULL;
    size_t word_len;

    while (true) {
        switch(token = get_token(input, input_len, &word, &word_len)) {
            case T_WORD:
                if (argc >= MAXARG - 1) {
                    fprintf(stderr, "Too many args\n");
                    continue;
                }
                if ((argv[argc] = malloc(word_len + 1)) == NULL) {
                    fprintf(stderr, "Out of arg memory\n");
                    continue;
                }
                memcpy(argv[argc], word, word_len);
                argv[argc][word_len] = '\0';
                ++argc;
                continue;

            case T_LT:
                if(makepipe) {
                    fprintf(stderr, "Extra <\n");
                    break;
                }
                if (get_token(input, input_len, &word, &word_len) != T_WORD) {
                    fprintf(stderr, "Illegal <\n");
                    break;
                }
                if ((srcfile = malloc(word_len + 1)) == NULL) {
                    fprintf(stderr, "Out of memory\n");
                    continue;
                }
                memcpy(srcfile, word, word_len);
                srcfile[word_len] = '\0';
                srcfd = -1;
                continue;

            case T_GT:
            case T_GTGT:
                if(dstfd != STDOUT_FILENO) {
                    fprintf(stderr, "Extra > or >>\n");
                    break;
                }
                if(get_token(input, input_len, &word, &word_len) != T_WORD) {
                    fprintf(stderr, "Illegal > or >>\n");
                    break;
                }
                if( (dstfile = malloc(word_len + 1)) == NULL) {
                    fprintf(stderr, "Out of memory\n");
                    continue;
                }
                memcpy(dstfile, word, word_len);
                dstfile[word_len] = '\0';
                dstfd = -1;
                append = token == T_GTGT;
                continue;

            case T_BAR:
            case T_AMP:
            case T_SEMI:
            case T_NL:
                argv[argc] = NULL;
                if (token == T_BAR) {
                    if (dstfd != STDOUT_FILENO) {
                        fprintf(stderr, "> or >> conflicts with |\n");
                        break;
                    }
                    term = command(input, input_len, wpid, true, &dstfd, shell);
                    if (term == T_ERROR){
                        if(dstfd != -1)
                            (void)close(dstfd);
                        goto cleanup;
                    }
                }
                else
                    term = token;

                if(makepipe){
                    if( pipe(pfd) == -1 )
                        goto cleanup;
                    *pipefdp = pfd[1];
                    srcfd = pfd[0];
                }

                if( (pid = invoke(argc, argv, srcfd, srcfile, dstfd, dstfile, append, term == T_AMP, pfd[1], shell)) == -1 )
                    goto cleanup;

                if(token != T_BAR)
                    *wpid = pid;
                if(argc == 0 && (token != T_NL || srcfd > 1))
                    fprintf(stderr, "Missing command\n");

                free(srcfile);
                free(dstfile);
                for(int i = 0; i < argc; ++i)
                    free(argv[argc]);
                return term;

            case T_EOF:
                shell->alive = false;
                break;
            case T_ERROR:
                goto cleanup;
        }
    }

    cleanup:
    free(srcfile);
    free(dstfile);
    for(int i = 0; i < argc; ++i)
        free(argv[argc]);
    return T_ERROR;
}

bool wait_and_display(pid_t pid){
    pid_t tmp;
    int status;
    do{
        if( (tmp = waitpid(-1, &status, 0)) == -1)
            return false;
        display_status(tmp, status);
    }while(tmp != pid);

    return true;
}

int shell_init(shell_t *shell){
    if(!shell){
        errno = EINVAL;
        return -1;
    }

    (void)memset(shell, 0, sizeof(*shell));
    shell->alive = true;
    set_sigenv(shell);
    return 0;
}

int shutdown_shell(shell_t *shell){
    if(!shell){
        errno = EINVAL;
        return -1;
    }

    shell->alive = false;
    restore_sigenv(shell);
    shutdown_server();
    return 0;
}

/*
int main(void){
    char *prompt = "> ";
    char *heh;
    size_t heh_len;

    size_t input_len = 0;
    char *input = NULL;
    TOKEN token = T_NL;

    alive = true;
    while(alive){
        if(token == T_NL){
            free(input);
            input_len = 0;
            printf("%s", prompt);
            fflush(stdout);
            if(getline(&input, &input_len, stdin) == -1)
                perror("user input lost");
        }

            switch (token = get_token(input, input_len, &heh, &heh_len)) {
                case T_WORD:
                    printf("T_WORD %.*s\t%d\n", heh_len, heh, heh_len);
                    break;
                case T_BAR:
                    printf("T_BAR\n");
                    break;
                case T_AMP:
                    printf("T_AMP\n");
                    break;
                case T_SEMI:
                    printf("T_SEMI\n");
                    break;
                case T_GT:
                    printf("T_GT\n");
                    break;
                case T_GTGT:
                    printf("T_GTGT\n");
                    break;
                case T_LT:
                    printf("T_LT\n");
                    break;
                case T_NL:
                    printf("T_NL\n");
                    break;
                case T_EOF:
                    printf("T_EOF\n");
                    exit(EXIT_SUCCESS);
                case T_ERROR:
                    printf("T_ERROR\n");
                    exit(EXIT_SUCCESS);
            }
    }
}
*/

