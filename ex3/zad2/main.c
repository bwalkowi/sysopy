#define _XOPEN_SOURCE 500
#define _POSIX_C_SOURCE 200112L

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>

#include <linux/limits.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <dirent.h>


bool extension(char *s1, char *s2){
	int n1 = strlen(s1);
	int n2 = strlen(s2);

	while((--n1 >= 0) && (--n2 >= 0)){
		if(*(s1 + n1) != *(s2 + n2))
			return false;
	}
	
	return true;
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
    if(!str){
        *arr = NULL;
        return 0;
    }

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

    if(!words){
        *arr = NULL;
        return 0;
    }

    *arr = malloc(words * sizeof(**arr));
    if (!(*arr))
        return -1;

    is_word = false;
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

int fnum(char **argv, bool v, bool w){

    char *dirpath = getenv("PATH_TO_BROWSE");

    struct stat fstat;
    if(lstat(dirpath, &fstat)){
        perror("lstat");
        return 0;
    }
    if(!S_ISDIR(fstat.st_mode)){
        return 0;
    }

    DIR *dir = opendir(dirpath);
    if (!dir)
        return 0;

    char **arr = NULL;
    char *ext = getenv("EXT_TO_BROWSE");
    char *tmp = NULL;

    if(ext){
        size_t len = strlen(ext);
        tmp = malloc((len + 1) * sizeof(*tmp));
        strncpy(tmp, ext, len);
        *(tmp + len) = '\0';
    }

    int words = split(tmp, ":", &arr);
    if(words < 0){
        free(tmp);
        if(closedir(dir))
            perror("closedir1");
        return 0;
    }

	char path[PATH_MAX];
    long pathlen = snprintf(path, PATH_MAX - 1, "%s/", dirpath);

    unsigned long files         =       0;
    unsigned long cfiles        =       0;
    unsigned long children      =       0;

    struct dirent *ent;
    while (errno = 0, ent = readdir(dir)){
        if (!strcmp(ent->d_name, ".") || !strcmp(ent->d_name, ".."))
            continue;

        long name = snprintf(path + pathlen, PATH_MAX - 1 - pathlen, "%s", ent->d_name);
        path[pathlen + name] = '\0';

        if(lstat(path, &fstat))
            perror("lstat1");
    
        if(S_ISDIR(fstat.st_mode)){
            pid_t pid = vfork();
            if(pid < 0)
                perror("vfork");

            else if(!pid){
                if(setenv("PATH_TO_BROWSE", path, 1)){
                    perror("setenv");
                    _exit(0);
                }

                if(execv("./fcounter", argv))
                    perror("execv");
                _exit(0);
            }

            else
                ++children;
        }

        else{
            if(!words)
                ++files;
            else{
                for(int i = 0; i < words; ++i){
                    if(extension(*(arr + i), ent->d_name)){
                        ++files;
                        break;
                    }
                }
            }
        }        
    }
    if(errno != 0)
        perror("readind dir");

    if(closedir(dir))
        perror("closedir2");        

    if(w)
        sleep(15);

    int status;
    for(unsigned long i = 0; i < children; ++i){
        wait(&status);
        if(WIFEXITED(status))
            cfiles += WEXITSTATUS(status);
    }

    if(v){
        path[pathlen] = '\0';
        printf("dir_path: %s\tfiles in dir: %ld\tfiles + cfiles: %ld\n", path, files, files + cfiles);
        fflush(stdout);
    }

    free(tmp);
    free(arr);
    return files + cfiles;
}

int main(int argc, char **argv){

    bool v  = false;
    bool w  = false;

    for(int i = 1; i < argc; ++i){

        if(!strcmp(*(argv + i), "-v"))
            v = true;
        else if(!strcmp(*(argv + i), "-w"))
            w = true;
        else if(!strcmp(*(argv + i), "-vw") || !strcmp(*(argv + i), "-wv"))
            v = w = true;
        else
            printf("Unrecognised option. Possible ones: [-v] [-w]\n");
    }

    if(setenv("PATH_TO_BROWSE", ".", 0)){
        perror("main setenv");
        return 0;
    }

    return fnum(argv, v, w);
}

