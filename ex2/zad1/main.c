#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>


int lib_sort(FILE *f, long size){
	char *a = malloc(size);
	if(!a){
		perror("Allocating buf1");
		return -1;
	}
	
	char *b = malloc(size);
	if(!b){
		perror("Allocating buf2");
		goto failed;
	}

	if(fseek(f, 0L, SEEK_END)){
		printf("fseek failed1");
		goto failed;
	}
	long sz = ftell(f);
	if(sz < 0){
		perror("ftell failed");
		return -1;
	}

	off_t min;
	for(off_t i = 0; i < sz - size; i += size){
		min = i;
		if(fseek(f, i, SEEK_SET)){
			printf("fseek failed2");
			goto failed;
		}
		if(fread(a, 1, 1, f) != 1){
			printf("read failed1");
			goto failed;
		}
		for(off_t j = i + size; j < sz; j += size){
			if(fseek(f, j, SEEK_SET)){
				printf("fseek failed3");
				goto failed;
			}
			if(fread(b, 1, 1, f) != 1){
				printf("read failed2");
				goto failed;
			}
			
			if((uint8_t)*b < (uint8_t)*a){
				min = j;
				*a = *b;
			}
		}

		if(min != i){
			if(fseek(f, i, SEEK_SET)){
				printf("fseek failed4");
				goto failed;
			}
			if(fread(a, size, 1, f) != 1){
				printf("read failed3\n");
				goto failed;
			}

			if(fseek(f, min, SEEK_SET)){
				printf("fseek failed5\n");
				goto failed;
			}
			if(fread(b, size, 1, f) != 1){
				printf("read failed4\n");
				goto failed;
			}

			if(fseek(f, i, SEEK_SET)){
				printf("fseek failed6\n");
				goto failed;
			}
			if(fwrite(b, size, 1, f) != 1){
				printf("write failed1");
				goto failed;
			}

			if(fseek(f, min, SEEK_SET)){
				printf("fseek failed7\n");
				goto failed;
			}
			if(fwrite(a, size, 1, f) != 1){
				printf("write failed2");
				goto failed;
			}
		}
	}

	return 0;

	failed:
		free(a);
		free(b);
		return -1;

}

int sys_sort(int f, long size){
	char *a = malloc(size);
	if(!a){
		perror("Allocating buf1");
		return -1;
	}
	
	char *b = malloc(size);
	if(!b){
		perror("Allocating buf2");
		goto failed;
	}

	off_t fsize = lseek(f, 0, SEEK_END);
	if(fsize == -1){
		perror("lseek failed1");
		goto failed;
	}

	off_t min;
	for(off_t i = 0; i < fsize; i += size){
		min = i;
		if(lseek(f, i, SEEK_SET) != i){
			perror("lseek failed2");
			goto failed;
		}
		if(read(f, a, 1) < 0){
			perror("read failed1");
			goto failed;
		}
		for(off_t j = i + size; j < fsize; j += size){
			if(lseek(f, j, SEEK_SET) != j){
				perror("lseek failed3");
				goto failed;
			}
			if(read(f, b, 1) < 0){
				perror("read failed2");
				goto failed;
			}
			
			if((uint8_t)*b < (uint8_t)*a){
				min = j;
				*a = *b;
			}
		}

		if(min != i){
			if(lseek(f, i, SEEK_SET) != i){
				perror("lseek failed4");
				goto failed;
			}
			if(read(f, a, size) < 0){
				perror("read failed3");
				goto failed;
			}

			if(lseek(f, min, SEEK_SET) != min){
				perror("lseek failed5");
				goto failed;
			}
			if(read(f, b, size) < 0){
				perror("read failed4");
				goto failed;
			}

			if(lseek(f, i, SEEK_SET) != i){
				perror("lseek failed6");
				goto failed;
			}
			if(write(f, b, size) < 0){
				perror("write failed1");
				goto failed;
			}

			if(lseek(f, min, SEEK_SET) != min){
				perror("lseek failed7");
				goto failed;
			}
			if(write(f, a, size) < 0){
				perror("write failed2");
				goto failed;
			}
		}
	}

	return 0;

	failed:
		free(a);
		free(b);
		return -1;

}

int main(int argc, char **argv){
	if(argc != 4){
		printf("\nIncorrect number of arguments.\nRequired one: prog filepath size func\n"
			   "\tprog - program name\n"
			   "\tfilepath - pathname to file to sort\n"
			   "\tsize - size of each struct\n"
			   "\tfunc - function variant, possible ones: sys/lib\n\n");
		return -1;
	}

	errno = 0;
	long size = strtol(*(argv + 2), NULL, 10);
	if(errno){
		perror("Reading size");
		return -1;
	}
	if(size <= 0){
		printf("size must be a positive number.\n");
		return -1;
	}

	int sorted;
	if(!strncmp(*(argv + 3), "sys", 4)){
		int fd = open(*(argv + 1), O_RDWR);
		if(fd < 0){
			perror("Opening file");
			return -1;
		}
		sorted = sys_sort(fd, size);
		if(close(fd) < 0){
			perror("Closing file");
			return -1;
		}
	}
	else if(!strncmp(*(argv + 3), "lib", 4)){
		FILE *fp = fopen(*(argv + 1), "r+b");
		if(!fp){
			perror("Opening file");
			return -1;
		}
		sorted = lib_sort(fp, size);
		if(fclose(fp)){
			perror("Closing file");
			return -1;
		}
	} 
	else{
		printf("func must be daclared as 'lib' or 'sys'.");
		return -1;
	}

	return sorted;
}

