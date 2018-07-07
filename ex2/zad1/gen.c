#include <time.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

ssize_t writeall(int fd, const void *buf, size_t nbyte){
	ssize_t nwritten = 0;
	ssize_t n;

	do{
		if((n = write(fd, &((const char *)buf) [nwritten], nbyte - nwritten)) == -1){
			if(errno == EINTR)
				continue;
			else
				return -1;
		}
		nwritten += n;
	} while(nwritten < nbyte);
	
	return nwritten;
}

int main(int argc, char **argv){
	if(argc != 4){
		printf("\nIncorrect number of arguments.\nRequired one: prog filename num size\n"
			   "\tprog - program name\n"
			   "\tfilename - name of file to create/overwrite\n"
			   "\tnum - number of structs to generate\n"
			   "\tsize - size of each struct\n\n");
		return -1;
	}

	errno = 0;
	long num = strtol(*(argv + 2), NULL, 10);
	if(errno){
		perror("Reading num");
		return -1;
	}
	if(num <= 0){
		printf("num must be a positive number.");
		return -1;
	}

	errno = 0;
	long size = strtol(*(argv + 3), NULL, 10);
	if(errno){
		perror("Reading size");
		return -1;
	}
	if(size <= 0){
		printf("size must be a positive number.");
		return -1;
	}

	srand(time(NULL));
	
	char *buf = malloc(size);
	if(!buf){
		perror("Allocating buf");
		return -1;
	}

	int f = open(*(argv + 1), O_CREAT|O_WRONLY|O_TRUNC | O_APPEND, S_IRWXU | S_IRWXG | S_IRWXO);
	if(f < 0){
		perror("Opening file");
		free(buf);
		return -1;
	}

	ssize_t written;
	for(long i = 0; i < num; ++i){
		*buf = (char)rand();
		written = writeall(f, buf, size);
		if(written < 0){
			perror("Writing to file");
			free(buf);
			close(f);
			return -1;
		}
	}

	free(buf);
	if(close(f) < 0){
		perror("Closing file");
		return -1;
	}
	
	return 0;
}

