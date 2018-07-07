#ifndef _SHELL_H_
#define _SHELL_H_

#define _POSIX_C_SOURCE 200809L
#define _XOPEN_SOURCE 500

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>

#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "ssi.h"

#define MAXARG 50

typedef enum {T_WORD, T_BAR, T_AMP, T_SEMI, T_GT, T_GTGT, T_LT, T_NL, T_EOF, T_ERROR} TOKEN;

typedef struct {
    bool alive;
    struct sigaction sig_int;
    struct sigaction sig_quit;
    struct sigaction sig_tstp;
    struct sigaction sig_ttin;
    struct sigaction sig_ttou;
    struct sigaction sig_chld;
} shell_t;


int shell_init(shell_t *);
int shutdown_shell(shell_t *);
TOKEN command(char *, size_t, pid_t *, bool, int *, shell_t *);

bool wait_and_display(pid_t);

#endif // _SHELL_H_

