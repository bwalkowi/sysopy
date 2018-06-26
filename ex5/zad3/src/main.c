#define _XOPEN_SOURCE

#include <stdio.h>
#include <string.h>

int main(int argc, char **argv){
	if(argc != 2){
		printf("Incorrect arguments. Required ones: prog n str\n"
               "\tprog - program name\n"
               "\tfpath - path to file to write output\n");
		return -1;
	}

    int len = strlen(*(argv + 1)) + 19;          // 1 for '\0' and 18 for 'ls -l | grep ^d > ' 
    char cmd[len];
    memcpy(cmd, "ls -l | grep ^d > ", 18);
    memcpy(cmd + 18, *(argv + 1), len - 18);

    FILE *fp = popen(cmd, "r");
    if(fp)
        pclose(fp);
    else
        printf("popen failed\n");

    return 0;
}

