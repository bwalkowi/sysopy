#define _XOPEN_SOURCE 500

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <stdbool.h>


static long max;
static char *str = NULL;

void quit(){
	printf("Odebrano sygnal SIGINT\n");
	exit(0);
}

void rev_str(char *str){
    if(str){
	    int length = strlen(str);
	    char sw;
	    for(int i=0; i < length/2; ++i){
	    	sw = str[i];
	    	str[i] = str[length - i - 1];
	    	str[length - i - 1] = sw;
    	}
    }
}

void print(){
    static bool rev = true;
    static unsigned int sig_num = 1;
    static int step = 1;

    if(sig_num == max)
        step = -1;
    else if(sig_num == 1)
        step = 1;

    rev = !rev;

    rev_str(str);
    for(int i = 0; i < sig_num; ++i)
        printf("%s\n", str);
    printf("\n");

    if(rev)
        sig_num += step;

}

int main(int argc, char **argv){
	if(argc != 3){
		printf("Incorrect arguments. Required ones: prog n str\n"
               "\tprog - program name\n"
               "\tn - max number of repetitions\n"
               "\tstr - string to print\n");
		return -1;
	}

    errno = 0;
    max = strtol(*(argv + 1), NULL, 10);
    if(errno){
        perror("offset must be a nonegative number");
        return -1;
    }

    size_t len = strlen(*(argv + 2));
    if(!(str = malloc((len + 1) * sizeof(*str)))){
        perror("malloc");
        return -1;
    }
    memcpy(str, *(argv + 2), len + 1);
	
    if(signal(SIGINT, quit) == SIG_ERR){
        perror("signal");
        goto cleanup;
    }

    sigset_t emptyset;
    if(sigemptyset(&emptyset)){
        perror("sigemptyset");
        goto cleanup;
    }

	struct sigaction sa;
	sa.sa_handler = print;
    sa.sa_mask = emptyset;
	sa.sa_flags = 0;
    if(sigaction(SIGTSTP, &sa, NULL)){
        perror("sigaction");
        goto cleanup;
    }

	while(true)
        sigsuspend(&emptyset);

	return 0;

    cleanup:
        free(str);
        return -1;
}

