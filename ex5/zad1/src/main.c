#define _XOPEN_SOURCE

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>

#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>


int err_exit(char *err_msg){
    perror(err_msg);
    exit(-1);
}

int main(int argc, char **argv){
	if(argc != 2){
		printf("Incorrect arguments. Required ones: prog n str\n"
               "\tprog - program name\n"
               "\tn - number of column for fold\n");
		return -1;
	}

    int len = strlen(*(argv + 1)) + 9;          // 1 for '\0' and 8 for 'fold -w ' 
    char fold_cmd[len];
    memcpy(fold_cmd, "fold -w ", 8);
    memcpy(fold_cmd + 8, *(argv + 1), len - 8);

    int fd[2];

    if(pipe(fd))
        err_exit("creating pipe");

    pid_t fold;

    if((fold = fork()) < 0)
        err_exit("fold vfork");

    else if(fold == 0){
        if(close(fd[1]))
            err_exit("fold close fd[1]");

        if(fd[0] != STDIN_FILENO){
            if(dup2(fd[0], STDIN_FILENO) == -1)
                err_exit("fold dup2");

            if(close(fd[0]))
                err_exit("fold close fd[0]");
        }

        execl("/bin/bash", "bash", "-c", fold_cmd, NULL);
        perror("exec");
        return -1;
    }
    else
        if(close(fd[0]))
            err_exit("tr close fd[0]");

    if(fd[1] != STDOUT_FILENO){
        if(dup2(fd[1], STDOUT_FILENO) == -1){
            perror("tr dup2");
            goto cleanup;
        }
        if(close(fd[1])){
            perror("tr close fd[1]");
            goto cleanup;
        }
    }

    execl("/bin/bash", "bash", "-c", "tr [:lower:] [:upper:]", NULL);
    perror("exec");

    cleanup:
        if(kill(fold, SIGKILL))
            perror("kill");

        int status;
        if(wait(&status) == -1)
            perror("wait");

        return -1;
}

