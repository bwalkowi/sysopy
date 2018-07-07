#ifndef DLL_LIST_H
#define DLL_LIST_H

#include <stdint.h>

enum {  NAME_LEN       =   31,
        SURNAME_LEN    =   31,
        EMAIL_LEN      =   31,
        ADDRESS_LEN    =   31,
        START_YEAR     =   1920,
        END_YEAR       =   2010,
        PHONE_LEAST    =   1000000     };


typedef struct DLL_Person {
    char name[NAME_LEN];
    char surname[SURNAME_LEN];
    struct{
        uint8_t day;
        uint8_t month;
        uint16_t year;
    } birth_date;
    char email[EMAIL_LEN];
    uint32_t phone;
    char address[ADDRESS_LEN];
} DLL_Person_t;


typedef struct DLL_Node{
    struct DLL_Node *next;
    struct DLL_Node *prev;
    DLL_Person_t person;
} DLL_Node_t;


typedef struct DLL_List{
    DLL_Node_t *head;
} DLL_List_t;


typedef int (*DLL_cmp_t)(DLL_Person_t *, DLL_Person_t *);

#ifndef DLL_LIB

void DLL_print_list(DLL_List_t *);

int DLL_cmp_all(DLL_Person_t *, DLL_Person_t *);

void DLL_sort(DLL_List_t *, DLL_cmp_t);

DLL_List_t* DLL_create_list(void);

void DLL_delete_list(DLL_List_t *);

DLL_Node_t* DLL_search(DLL_List_t *, DLL_Person_t *, DLL_cmp_t);

int DLL_add_elem(DLL_List_t *, DLL_Person_t *);

void DLL_remove_elem(DLL_List_t *, DLL_Node_t *);

DLL_Person_t* DLL_create_person(void);

int DLL_set_person(DLL_Person_t *, char *, char *, uint8_t, uint8_t, uint16_t, char *, uint32_t, char *);

#endif

#endif //DLL_LIST_H
