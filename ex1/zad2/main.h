#ifndef MAIN_H
#define MAIN_H

enum {  ERROR_INCORRECT_NUM_OF_ARGS     =   -1,
        ERROR_OUT_OF_MEMORY             =   -2,
        ERROR_COULD_NOT_OPEN_FILE       =   -3,
        ERROR_TEST_FAILED               =   -4,
	ERROR_COULD_NOT_LOAD_LIB	=   -5      };


int split(char *, char *, char ***);

bool is_in(char, char *);

#endif
