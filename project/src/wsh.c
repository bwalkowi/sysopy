#include "shell.h"


static int run_shell(){
    char *prompt = "> ";
    pid_t pid = 0;
    TOKEN term = T_NL;

    size_t input_len = 0;
    char *input = NULL;

    shell_t shell;
    if(shell_init(&shell) == -1){
        perror("init shell");
        return -1;
    }

    while(shell.alive){
        if(term == T_NL){
            free(input);
            input_len = 0;
            printf("%s", prompt);
            fflush(stdout);
            if(getline(&input, &input_len, stdin) == -1)
                break;
        }

        term = command(input, input_len, &pid, false, NULL, &shell);
        if(term == T_ERROR) {
            fprintf(stderr, "Bad command\n");
            term = T_NL;
        }
        if(term != T_AMP && pid > 0)
            wait_and_display(pid);
    }

    shutdown_shell(&shell);
    return 0;
}

int main(void){
    return run_shell() == -1 ? -1 : 0;
}

