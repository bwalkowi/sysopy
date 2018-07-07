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
#include <stdlib.h>

#include <ftw.h>

static mode_t mode = 0;

int print_entry(const char *filepath, const struct stat *info, int typeflag, struct FTW *pathinfo){
	if(typeflag == FTW_F && ((info->st_mode & (S_IRWXU | S_IRWXG | S_IRWXO)) == mode)){

		char buffer[20];
		struct tm *timeinfo;
		timeinfo = localtime(&(info->st_atime));
		strftime(buffer, 20, "%Y %b %d %H:%M", timeinfo);
		
		char buf[PATH_MAX];
		if(!realpath(filepath, buf)){
            perror("Realpath err");
            return -1;
        }
		
		printf("%s\t\t%lld\t %s\n", buf, (long long)info->st_size, buffer);
	}
	
	return 0;
}

int lsdir_nftw(char *dirpath, mode_t mode){
	if(nftw(dirpath, print_entry, 10, FTW_PHYS | FTW_MOUNT))
		return -1;

	return 0;
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

    mode = 0;
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

	if(lsdir_nftw(*(argv + 1), mode) == -1){
		perror("Error");
		return -1;
	}

	return 0;
}

