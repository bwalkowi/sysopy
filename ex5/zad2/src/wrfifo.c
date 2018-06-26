#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include <fcntl.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


int err_exit(char *err_msg){
    perror(err_msg);
    exit(-1);
}

int main(int argc, char **argv){
	if(argc != 2){
		printf("Incorrect arguments. Required ones: prog n str\n"
               "\tprog - program name\n"
               "\tfname - fifo name\n");
		return -1;
	}

    int fd;
    if((fd = open(*(argv + 1), O_WRONLY)) == -1)
        err_exit("open fifo to write failed");

    int len;
    size_t buf_size = 1024;
    char buffer[buf_size];
    struct tm *tm_info;
    time_t timer;

    while(true){
        sprintf(buffer, "%d", getpid());
        len = strlen(buffer);
        buffer[len] = ' ';
        buffer[len + 1] = '-';
        buffer[len + 2] = ' ';

        fgets(buffer + len + 3 + 11, buf_size - len - 3 - 11, stdin);

        time(&timer);
        tm_info = localtime(&timer);
        strftime(buffer + len + 3, 9, "%H:%M:%S", tm_info);
        len = strlen(buffer);
        buffer[len] = ' ';
        buffer[len + 1] = '-';
        buffer[len + 2] = ' ';

        len = strlen(buffer);
        write(fd, buffer, len + 1);
    }

    return 0;
}

