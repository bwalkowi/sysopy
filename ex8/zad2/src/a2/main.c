#define _XOPEN_SOURCE 500

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <stdatomic.h>

#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define SUCCES ( (void *) 0)
#define FAILURE ( (void *) -1)

#define RECORD_LEN 1024
#define TEXT_LEN (RECORD_LEN - sizeof(int))

typedef struct{
    volatile atomic_long n;
    long k;
    int fd;
    char *word;
    pthread_mutex_t mtx;
    pthread_t *threads;    
} reader_t;

typedef struct{
    reader_t *reader;
    char *buf;
} thread_t;

typedef struct{
    int id;
    char text[TEXT_LEN];
} record_t;

void cleanup(reader_t *reader){
    (void)pthread_mutex_destroy(&reader->mtx);
    (void)close(reader->fd);
    free(reader->threads);
    free(reader);
}

void thread_cleanup(void *arg){
    thread_t *thread = arg;
    free(thread->buf);

    if( (atomic_fetch_sub(&thread->reader->n, 1) - 1) == 0)
        cleanup(thread->reader);
}

ssize_t readall(int fd, char *buf, size_t nbyte){
    ssize_t nread = 0, n;
    do{
        if( (n = read(fd, (void *)&buf[nread], nbyte - nread)) == -1){
            if(errno == EINTR)
                continue;
            else
                return -1;
        }
        if(n == 0)
            return nread;
        nread += n;
    } while(nread < nbyte);
    return nread;
}

int find_word(char* line, char* word) {
    int line_len = strlen(line);
    int word_len = strlen(word);
    bool is_word = false;

    for(int i = 0; i <= line_len - word_len; ++i) {
        if(line[i] == ' ' || line[i] == '\t' || line[i] == '\n')
            is_word = false;
        else if(!is_word){
            is_word = true;
            if(line[i] == word[0]) {
                for(int j = 0; j < word_len; ++j) {
                    if(line[i + j] != word[j]) 
                        break;
                    if(j == word_len - 1 && (line[i + j + 1] == ' ' || line[i + j + 1] == '\t' || line[i + j + 1] == '\n')) 
                        return i;
                }
            }
        }
    }

    return -1;
}

void *thread_func(void *arg){
    reader_t *reader = arg;
    char *buf = NULL;
    thread_t thread = {.reader = reader, .buf = buf};
    pthread_cleanup_push(thread_cleanup, &thread);
    pthread_detach(pthread_self());

    if( (buf = calloc(reader->k * RECORD_LEN, sizeof(*buf))) == NULL){
        perror("allocating buf:");
        pthread_exit(FAILURE);
    }

    ssize_t nread = 0;
    bool eof = false;
    while(!eof){

        pthread_mutex_lock(&reader->mtx);
        nread = readall(reader->fd, buf, reader->k * RECORD_LEN);
        pthread_mutex_unlock(&reader->mtx);

        if(nread == -1){
            perror("reading");
            pthread_exit(FAILURE);
        }
        else if(nread == 0)
            eof = true;

        else{
            record_t *record;
            for(int i = 0; i < nread / RECORD_LEN; ++i){
                record = (record_t *)&buf[i * RECORD_LEN];
                if(find_word(record->text, reader->word) != -1)
                    printf("Thread: %ld: found %d\n", pthread_self(), record->id);
            }
        }
    }

    pthread_cleanup_pop(1);
    pthread_exit(SUCCES);
}

int main(int argc, char **argv){
    if(argc != 6){
        printf("Incorrect arguments. Required ones: prog n fname k word signal\n"
                   "\tprog - program name\n"
                   "\tn - number of threads\n"
                   "\tfname - name of file\n"
                   "\tk - number of record to read\n"
                   "\tword - word to find\n"
                   "\tsig - sig to send\n");
        return -1;
    }

    errno = 0;
    long n = strtol(argv[1], NULL, 10);
    long k = strtol(argv[3], NULL, 10);
    if(errno || n < 1 || k < 1){
        perror("number of threads and number of record to read must be a nonegative number");
        return -1;
    }

    int sig;
    switch(argv[5][0]){
        case 'a':
            sig = SIGUSR1;
            break;
        case 'b':
            sig = SIGTERM;
            break;
        case 'c':
            sig = SIGKILL;
            break;
        case 'd':
            sig = SIGSTOP;
            break;
        default:
            printf("Incorrect sig argument\n");
            return -1;
    }

    reader_t *reader = malloc(sizeof(*reader));
    if(reader == NULL){
        perror("alloc reader");
        return -1;
    }
    atomic_init(&reader->n, 1);
    reader->k = k;
    reader->word = argv[4];
    if( (reader->threads = malloc(n * sizeof(*reader->threads))) == NULL){
        perror("alloc arr");
        free(reader);
        return -1;
    }

    if( (reader->fd = open(argv[2], O_RDONLY)) == -1){
        perror("opening file");
        free(reader->threads);
        free(reader);
        return -1;
    }

    int err;
    if( (err = pthread_mutex_init(&reader->mtx, NULL))  != 0){
        printf("init mutex: %s\n", strerror(err));
        cleanup(reader);
    }

    pthread_mutex_lock(&reader->mtx);

    reader->threads[0] = pthread_self();    
    for(int i = 1; i < n; ++i){
        if((err = pthread_create(&reader->threads[i], NULL, thread_func, reader)))
            printf("creating thread failed: %s\n", strerror(err));
        else
            atomic_fetch_add(&reader->n, 1);
    }

    sigset_t set;
    sigfillset(&set);
    pthread_sigmask(SIG_SETMASK, &set, NULL);

    pthread_mutex_unlock(&reader->mtx);

    pthread_detach(pthread_self());
    kill(getpid(), sig);
    (void)thread_func(reader);
}

