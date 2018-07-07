#define _XOPEN_SOURCE 500

#include <stdio.h>
#include <errno.h>
#include <dirent.h>
#include <string.h>

#include <linux/limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <time.h>
#include <limits.h>
#include <stdlib.h>


int lsdir(char* dirpath, mode_t mode){
	char err_msg[100];
	DIR *dir;
	
	if(!(dir = opendir(dirpath))){
		snprintf(err_msg, sizeof err_msg, "Cannot open directory '%s'", dirpath);
		perror(err_msg);
		return -1;
	}
	
    	struct dirent *entry;
	while(errno = 0, (entry = readdir(dir))){
		
		long pathlen;
		char path[PATH_MAX];
       		path[PATH_MAX - 1] = '\0';
				
		pathlen = snprintf(path, PATH_MAX - 1, "%s/%s", dirpath, entry->d_name);
		
		struct stat fstat;
		if(lstat(path, &fstat)){
        	    perror("lstat failed");
        	    goto cleanup;
        	}
		
		if(S_ISREG(fstat.st_mode) && ((fstat.st_mode & (S_IRWXU | S_IRWXG | S_IRWXO)) == mode)){
			char buffer[20];
			struct tm *timeinfo;
			timeinfo = localtime(&(fstat.st_atime));
	
			strftime(buffer, 20, "%Y %b %d %H:%M", timeinfo);
			printf("%s/%s\t\t%lld\t %s\n", dirpath, entry->d_name,  
				                             (long long)fstat.st_size, buffer);
		}
		
		if(S_ISDIR(fstat.st_mode)){
			if(strncmp(entry->d_name, "..", 3) && strncmp(entry->d_name, ".", 2)){
				if(pathlen >= PATH_MAX){
					fprintf(stderr, "Path length is too long.\n");
					perror("");
					return -1;
				}
				
				lsdir(path, mode);			
			}
		}
	}
    if(errno){
        perror("Reading dir");
        goto cleanup;
    }
	
	if(closedir(dir)){
		snprintf(err_msg, sizeof err_msg, "Cannot close directory '%s'", dirpath);
		perror(err_msg);
		return -1;
	}
	
	return 0;

    cleanup:
        closedir(dir);
        return -1;
}

int main(int argc, char **argv){
	if(argc != 3){
        printf("Incorrect arguments. Required ones: prog path mode\n"
               "\tprog - program name.\n" 
               "\tpath - dir where to begin with.\n"
               "\tmode - acces mode in octal format e.g. 777\n");
        return -1;
    }  

    if(strlen(*(argv + 2)) != 3){
        printf("Incorrect mode.\n");
        return -1;
    }

    mode_t modes[3][3] = { {S_IRUSR, S_IWUSR, S_IXUSR}, 
                           {S_IRGRP, S_IWGRP, S_IXGRP}, 
                           {S_IROTH, S_IWOTH, S_IXOTH}  };
    mode_t mode = 0;
    for(int i = 0; i < 3; ++i){
        char c = *(*(argv + 2) + i) - '0';
        if(c < 0 || c > 7){
            printf("Incorrect mode.\n");
            return -1;
        }
        if(c >= 4){
            c -= 4;
            mode |= modes[i][0];
        }
        if(c >= 2){
            c -= 2;
            mode |= modes[i][1];
        }
        if(c >= 1)
            mode |= modes[i][2];
    }

    if(chdir(*(argv + 1)) < 0){
	perror("Could not open dir");
	return -1;
    }
    
    return lsdir(".", mode); 
}
