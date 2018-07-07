#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>

typedef struct lock_t {
    struct lock_t *next;
    off_t byte;
    int type;
} lock_t;

int add_elem(lock_t **l, off_t byte, int type){
	lock_t *tmp = malloc(sizeof(*tmp));
    if(!tmp)
        return -1;

    tmp->byte = byte;
    tmp->type = type;
    tmp->next = *l;
    *l = tmp;
    return 0;
}

void delete(lock_t *l){
    if(l){
        lock_t *p = l->next;
        free(l);
        while(p){
            l = p;
            p = p->next;
            free(l);
        }
    }
}

int search(lock_t *l, off_t byte){
    while(l){
        if(l->byte == byte)
            return l->type;
        l = l->next;
    }
    return -1;
}

void del_elem(lock_t **l, off_t byte){
    if(*l){
        if((*l)->byte == byte){
            lock_t *p = *l;   
            *l = (*l)->next;
            free(p);
        }
        else{
            lock_t *q = *l;
            while(q->next){
                if(q->next->byte == byte){
                    lock_t *p = q->next;
                    q->next = q->next->next;
                    free(p);
                }
                q = q->next;
            }
        }
    }
}

int lock_reg(int fd, int cmd, int type, off_t offset, int whence, off_t len, lock_t **l){
	struct flock lock;

	lock.l_type = type;
	lock.l_start = offset;
	lock.l_whence = whence;
	lock.l_len = len;

    if(fcntl(fd, cmd, &lock) == -1)
        return -1;

    if(type == F_UNLCK){
        del_elem(l, offset);
           return 0;
    }
	else
        return add_elem(l, offset, type);
}

pid_t lock_owner(int fd, int type, off_t offset, int whence, int len){
	struct flock lock;

	lock.l_type = type;
	lock.l_start = offset;
	lock.l_whence = whence;
	lock.l_len = len;

	if(fcntl(fd, F_GETLK, &lock) < 0){
		perror("Fcntl failed2");
		return -1;
	}

	if(lock.l_type == F_UNLCK)
		return 0;
	else
		return lock.l_pid;
}

int lock_type(int fd, int byte){
	struct flock lock;

	lock.l_type = F_WRLCK;
	lock.l_start = byte;
	lock.l_whence = SEEK_SET;
	lock.l_len = 1;
	if(fcntl(fd, F_GETLK, &lock) < 0){
		perror("Fcntl failed1");
		return -1;
	}

	return lock.l_type;
}

void test_bytes(int fd, lock_t *l){
	off_t end = lseek(fd, 0, SEEK_END);

	for(off_t i = 0 ; i < end ; i++){
		int type = lock_type(fd, i);
		if(type == -1){
			printf("%ld byte checking failed.\n", i);
			continue;
		}
		else if(type == F_WRLCK)
			printf("%ld blocked \t F_WRLCK \t PID=%d\n", i, lock_owner(fd, F_WRLCK, i, SEEK_SET, 1));
		else if(type == F_RDLCK)
			printf("%ld blocked \t F_RDLCK \t PID=%d\n", i, lock_owner(fd, F_RDLCK, i, SEEK_SET, 1));
	}

    pid_t tmp = getpid();
    while(l){
        printf("%ld blocked \t %s \t PID=%d\n", l->byte, l->type == F_WRLCK ? "F_WRLCK" : "F_RDLCK", tmp);
        l = l->next;
    }
}

bool is_in(char c, char *list){
    while(*list != '\0'){
        if(c == *list)
            return true;
        ++list;
    }
    return false;
}

int split(char *str, char *delim, char ***arr){

    int words = 0;
    bool is_word = false;
    char *p = str - 1;

    while (*++p != '\0') {

        if (is_in(*p, delim))
            is_word = false;

        else if (!is_word) {
            is_word = true;
            ++words;
        }
    }

    *arr = malloc(words * sizeof(**arr));
    if (!(*arr))
        return -1;

    int i = 0;
    p = str - 1;
    while (*++p != '\0') {

        if (is_in(*p, delim)) {
            *p = '\0';
            is_word = false;
        }
        else if (!is_word) {
            is_word = true;
            *(*arr + i) = p;
            ++i;
        }
    }

    return words;
}

int main(int argc, char **argv){

	if(argc != 2){
		printf("Incorrect arguments. Required ones: prog fname\n"
               "\tprog - program name\n"
               "\tfname - filename\n");
		return -1;
	}

	int fd = open(*(argv + 1), O_RDWR);
	if(fd == -1){
		perror("Opening file");
        return -1;
	}

    char *commands = "help\t\t\tshow help\n"
	                 "quit\t\t\tquit program\n"
	                 "set read [n]\t\tset read lock on n-th byte\n"
	                 "set write [n]\t\tset write lock on n-th byte\n"
                     "set unlock [n]\t\tunlock n-th byte\n"
	                 "read [n]\t\tread n-th byte\n"
	                 "write [n] [sign]\twrite sign to n-th byte\n"
                     "show\t\t\tprint every lock in file\n";

	printf("\nType \"help\" to see avaible options\n");

    int bufsize = 256;
//	char *buf;
    char buf[bufsize];

    lock_t *my_locks = NULL;

	while(true){
     //   buf = malloc(bufsize * sizeof(char));
		printf(">> ");

//	    scanf("%100s", buf);
        if(!fgets(buf, bufsize, stdin)){
           if(ferror(stdin))
                printf("Error reading input\n");
            continue;
        }

//        printf("%s", buf);
        char **cmd;
        int words = split(buf, " \n", &cmd);
        if(words > 3 || words < 1){
            printf("Incorrect command format. Type \"help\" for help\n");
            free(cmd);
            continue;
        }
//        printf("%d", words);
//        printf("%s", *cmd);

//        printf("%s", *(cmd+1));

//        printf("po ifie\n");

        if(!strncmp(*cmd, "quit", 4) && words == 1){
            free(cmd);
            break;
        }

		if(!strncmp(*cmd, "help", 5) && words == 1){
			printf("%s", commands);
            free(cmd);
            continue;
        }

		else if(!strncmp(*cmd, "set", 4) && words >= 3){

        	errno = 0;
        	long offset = strtol(*(cmd + 2), NULL, 10);
        	if(errno){
        		perror("offset must be a nonegative number");
                free(cmd);
        		continue;
        	}
        	if(offset < 0){
        		printf("offset must be a nonegative number.\n");
                free(cmd);
           		continue;
        	}

			if(!strncmp(*(cmd + 1), "read", 5)){
				int pid;
				if(!(pid = lock_owner(fd, F_RDLCK, offset, SEEK_SET, 1))){
					if(lock_reg(fd, F_SETLK, F_RDLCK, offset, SEEK_SET, 1, &my_locks) < 0)
						printf("Blocking failed\n");
				}
				else printf("Cannot be blocked: PID=%d\n", pid);
			}

			else if(!strncmp(*(cmd + 1), "write", 6)){
				int pid;
				if(!(pid = lock_owner(fd, F_WRLCK, offset, SEEK_SET, 1))){
					if(lock_reg(fd, F_SETLK, F_WRLCK, offset, SEEK_SET, 1, &my_locks) < 0)
						printf("Blocking failed\n");
				}
				else printf("Cannot be blocked: PID=%d\n", pid);
			}

			else if(!strncmp(*(cmd + 1), "unlock", 7))
				if(lock_reg(fd, F_SETLK, F_UNLCK, offset, SEEK_SET, 1, &my_locks) < 0)
					printf("Unblocking failed\n");
		}

		else if(!strncmp(*cmd, "show", 5) && words == 1)
			test_bytes(fd, my_locks);

		else if(!strncmp(*cmd, "read", 5) && words == 2){

        	errno = 0;
        	long offset = strtol(*(cmd + 1), NULL, 10);
        	if(errno){
        		perror("offset must be a nonegative number");
                free(cmd);
        		continue;
        	}
        	if(offset < 0){
        		printf("offset must be a nonegative number.\n");
                free(cmd);
        		continue;
        	}

            char a;
			if(lock_type(fd, offset) != F_RDLCK){
    			if(lseek(fd, offset, SEEK_SET) != offset){
    				perror("read/lseek failed");
                    free(cmd);
    				continue;
    			}
    			if((read(fd, &a, 1)) < 0){
    				perror("read failed");
                    free(cmd);
    				continue;
    			}
                printf("%c was read.\n", a);
            }
			else 
                printf("Cannot read - locked\n");
        }

		else if(!strncmp(*cmd, "write", 6) && words == 3){
			
        	errno = 0;
        	long offset = strtol(*(cmd + 1), NULL, 10);
        	if(errno){
        		perror("offset must be a nonegative number");
                free(cmd);
        		continue;
        	}
        	if(offset < 0){
        		printf("offset must be a nonegative number.\n");
                free(cmd);
        		continue;
        	}
            char c = **(cmd + 2);
			
			if(lock_type(fd, offset) != F_WRLCK && lock_type(fd, offset) != F_RDLCK){
    			if(lseek(fd, offset, SEEK_SET) != offset){
    				perror("write/lseek failed");
                    free(cmd);
    				continue;
    			}
       			if(write(fd, &c, 1) < 0){
    				perror("write failed");
                    free(cmd);
    				continue;
    			}
            }
			else 
                printf("Cannot write - locked\n");			
		}

		else
            printf("Unrecognized command. For \"help\" type help\n");

        free(cmd);

 //       free(buf);
	}

    delete(my_locks);
	if(close(fd)){
        perror("Closing file");
        return -1;
    }

	return 0;
}
