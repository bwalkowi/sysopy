#include <time.h>
#include <sys/times.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "main.h"

#include "list.h"

#ifdef DLL_LIB
#include <dlfcn.h>
#endif

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

int main(int argc, char **argv) {

    struct rusage rusage;
    clock_t my_clock;
    getrusage(RUSAGE_SELF, &rusage);
    my_clock = clock();
    double real1 = (double)my_clock / CLOCKS_PER_SEC;
    double user1 = (double)rusage.ru_utime.tv_sec + (double)rusage.ru_utime.tv_usec / (double)10e6;
    double sys1 = (double)rusage.ru_stime.tv_sec + (double)rusage.ru_stime.tv_usec / (double)10e6;

    printf("1st CHECKPOINT\n<==========================>\nCURRENT TIME\n");
    printf("real time:\t%.10f\nuser time:\t%.10f\nsys time:\t%.10f\n<==========================>\n\n", 
            real1, user1, sys1);

    if(argc != 2)
	return ERROR_INCORRECT_NUM_OF_ARGS;

    #ifdef DLL_LIB

	void *handle = dlopen("../zad1/liblist2.so", RTLD_LAZY);

	void (*DLL_print_list)(DLL_List_t *) = dlsym(handle, "DLL_print_list");

	int (*DLL_cmp_all)(DLL_Person_t *, DLL_Person_t *) = dlsym(handle, "DLL_cmp_all");

	void (*DLL_sort)(DLL_List_t *, DLL_cmp_t) = dlsym(handle, "DLL_sort");

	DLL_List_t* (*DLL_create_list)(void) = dlsym(handle, "DLL_create_list");

	void (*DLL_delete_list)(DLL_List_t *) = dlsym(handle, "DLL_delete_list");

	DLL_Node_t* (*DLL_search)(DLL_List_t *, DLL_Person_t *, DLL_cmp_t) = dlsym(handle, "DLL_search");

	int (*DLL_add_elem)(DLL_List_t *, DLL_Person_t *) = dlsym(handle, "DLL_add_elem");

	void (*DLL_remove_elem)(DLL_List_t *, DLL_Node_t *) = dlsym(handle, "DLL_remove_elem");

	DLL_Person_t* (*DLL_create_person)(void) = dlsym(handle, "DLL_create_person");

	int (*DLL_set_person)(DLL_Person_t *, char *, char *, uint8_t, uint8_t, uint16_t, char *, uint32_t, char *) = dlsym(handle, "DLL_set_person");

	if(!handle || !DLL_print_list || !DLL_cmp_all || !DLL_sort || !DLL_create_list || 
		!DLL_delete_list || !DLL_search || !DLL_add_elem || !DLL_remove_elem || 
		!DLL_create_person || !DLL_set_person)
	    return ERROR_COULD_NOT_LOAD_LIB;

    #endif

    FILE *fp = fopen(*(argv + 1), "r");
    if(!fp)
        return ERROR_COULD_NOT_OPEN_FILE;

    int error = 0;

    int line_size = 200;
    char *line_buf = malloc(line_size * sizeof(*line_buf));

    DLL_Person_t *p = DLL_create_person();
    DLL_List_t *list = DLL_create_list();

    if(!p || !list || !line_buf) {
        error = ERROR_OUT_OF_MEMORY;
        goto end;
    }

    int expected = 8;

    uint8_t day;
    uint8_t month;
    uint16_t year;
    uint32_t phone;

    char *delim = " \"\n";
    char **tokens;

    int i = 0;

    my_clock = clock();
    getrusage(RUSAGE_SELF, &rusage);
    double real2 = (double)my_clock / CLOCKS_PER_SEC;
    double user2 = (double)rusage.ru_utime.tv_sec + (double)rusage.ru_utime.tv_usec / (double)10e6;
    double sys2 = (double)rusage.ru_stime.tv_sec + (double)rusage.ru_stime.tv_usec / (double)10e6;

    printf("2nd CHECKPOINT\n<==========================>\nCURRENT TIME\n");
    printf("real time:\t%.10f\nuser time:\t%.10f\nsys time:\t%.10f\n--------------------------\n(CURRENT - PREVIOUS) TIME\n", 
           real2, user2, sys2);
    printf("real time:\t%.10f\nuser time:\t%.10f\nsys time:\t%.10f\n<==========================>\n\n", 
	   real2 - real1, user2 - user1, sys2 - sys1);

    while (fgets(line_buf, line_size, fp)){
        ++i;
        printf("%s\n", line_buf);

        int words = split(line_buf, delim, &tokens);
        if(words < 0){
            error = ERROR_OUT_OF_MEMORY;
            goto end;
        }
        if(words < expected){
            printf("Line %d: incorrect format.\n", i);
            continue;
        }

        day = (uint8_t)atoi(*(tokens + 2));
        month = (uint8_t)atoi(*(tokens + 3));
        year = (uint16_t)atoi(*(tokens + 4));
        phone = (uint32_t)atoi(*(tokens + 6));
        if(!day || !month || !year || !phone) {
            printf("Line %d: incorrect arguments.\n", i);
            continue;
        }

        char *temp = *(tokens + 7);
        while(temp != *(tokens + words - 1)){
            if(*temp == '\0')
                *temp = ' ';

            ++temp;
        }

        if(DLL_set_person(p, *tokens, *(tokens + 1), day, month, year, *(tokens + 5), phone, *(tokens + 7))){
            printf("Line %d: incorrect argumentss.\n", i);
            continue;
        }

        if(DLL_add_elem(list, p)){
            error = ERROR_OUT_OF_MEMORY;
            goto end;
        }

        DLL_Node_t *tmp = DLL_search(list, p, DLL_cmp_all);
        if(!tmp || DLL_cmp_all(p, &tmp->person)){
            error = ERROR_TEST_FAILED;
            goto end;
        }
        DLL_print_list(list);

        DLL_sort(list, DLL_cmp_all);
        DLL_print_list(list);

        if(i % 3 == 0){
            DLL_remove_elem(list, tmp);
            
        }
        DLL_print_list(list);
    }

    my_clock = clock();
    getrusage(RUSAGE_SELF, &rusage);
    double real3 = (double)my_clock / CLOCKS_PER_SEC;
    double user3 = (double)rusage.ru_utime.tv_sec + (double)rusage.ru_utime.tv_usec / (double)10e6;
    double sys3 = (double)rusage.ru_stime.tv_sec + (double)rusage.ru_stime.tv_usec / (double)10e6;

    printf("3rd CHECKPOINT\n<==========================>\nCURRENT TIME\n");
    printf("real time:\t%.10f\nuser time:\t%.10f\nsys time:\t%.10f\n--------------------------\n(CURRENT - PREVIOUS) TIME\n", 
    	   real3, user3, sys3);
    printf("real time:\t%.10f\nuser time:\t%.10f\nsys time:\t%.10f\n--------------------------\n(CURRENT - 1st) TIME\n", 
	   real3 - real2, user3 - user2, sys3 - sys2);
    printf("real time:\t%.10f\nuser time:\t%.10f\nsys time:\t%.10f\n<==========================>\n\n", 
	   real3 - real1, user3 - user1, sys3 - sys1);

    end:

    DLL_delete_list(list);
    free(p);
    free(line_buf);
    fclose(fp);

    #ifdef DLL_LIB

    	dlclose(handle);

    #endif

    my_clock = clock();
    getrusage(RUSAGE_SELF, &rusage);
    double real4 = (double)my_clock / CLOCKS_PER_SEC;
    double user4 = (double)rusage.ru_utime.tv_sec + (double)rusage.ru_utime.tv_usec / (double)10e6;
    double sys4 = (double)rusage.ru_stime.tv_sec + (double)rusage.ru_stime.tv_usec / (double)10e6;

    printf("4th CHECKPOINT\n<==========================>\nCURRENT TIME\n");
    printf("real time:\t%.10f\nuser time:\t%.10f\nsys time:\t%.10f\n--------------------------\n(CURRENT - PREVIOUS) TIME\n", 
   	   real4, user4, sys4);
    printf("real time:\t%.10f\nuser time:\t%.10f\nsys time:\t%.10f\n--------------------------\n(CURRENT - 1st) TIME\n", 
	   real4 - real3, user4 - user3, sys4 - sys3);
    printf("real time:\t%.10f\nuser time:\t%.10f\nsys time:\t%.10f\n<==========================>\n\n", 
	   real4 - real1, user4 - user1, sys4 - sys1);

    return error;
}
