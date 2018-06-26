#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define RECORD_LEN 1024
#define TEXT_LEN (RECORD_LEN - sizeof(int))

typedef struct{
    int id;
    char text[TEXT_LEN];
} record_t;

int main(){
    srand(time(NULL));
    int fd = open("records.txt", O_CREAT|O_WRONLY|O_TRUNC | O_APPEND, S_IRWXU | S_IRWXG | S_IRWXO);
    int n =1000;
    record_t record;
    for(int i = 0; i < n; ++i){
        record.id = i;
        for(int j = 0; j < TEXT_LEN - 2; ++j){
            record.text[j] = 'a'+(rand()%('z'-'a'));
            if(! (rand()%10))
                record.text[j] = ' ';
        }
        record.text[TEXT_LEN - 2] = '\n';
        record.text[TEXT_LEN - 1] = '\0';
        write(fd, (void *)&record, sizeof(record));
    }
    close(fd);
    return 0;
}
