#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>


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

    if(mkfifo(*(argv + 1), S_IRWXU))
        err_exit("mkfifo failed");

    int fd;
    if((fd = open(*(argv + 1), O_RDONLY)) == -1)
        err_exit("open fifo to read failed");

    int len;
    size_t buf_size = 1024;
    char buffer[buf_size];
    struct tm *tm_info;
    time_t timer;

    while(true){
        if((len = read(fd, buffer + 11, buf_size - 11)) > 0 && (time(&timer)) > 0){
            tm_info = localtime(&timer);
            strftime(buffer, 9, "%H:%M:%S", tm_info);
            buffer[8] = ' ';
            buffer[9] = '-';
            buffer[10] = ' ';
            printf("%s\n", buffer);
        }
    }

    return 0;
}

