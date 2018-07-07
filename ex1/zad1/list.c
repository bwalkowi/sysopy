#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "list.h"


DLL_List_t* DLL_create_list(void){
    return calloc(1, sizeof(DLL_List_t));
}

void DLL_delete_list(DLL_List_t *list){
    if(list->head){
        while(list->head->next){
            list->head = list->head->next;
            free(list->head->prev);
        }
        free(list->head);
    }
    free(list);
}

DLL_Node_t* DLL_search(DLL_List_t *list, DLL_Person_t *p, DLL_cmp_t cmp){
    DLL_Node_t *tmp = list->head;
    while(tmp){
        if(!cmp(p, &tmp->person))
            break;

        tmp = tmp->next;
    }

    return tmp;
}

int DLL_add_elem(DLL_List_t *list, DLL_Person_t *p){
    DLL_Node_t *tmp = malloc(sizeof(*tmp));
    if(!tmp)
        return -1;

    tmp->person.birth_date.day = p->birth_date.day;
    tmp->person.birth_date.month = p->birth_date.month;
    tmp->person.birth_date.year = p->birth_date.year;
    tmp->person.phone = p->phone;
    strncpy(tmp->person.name, p->name, NAME_LEN);
    strncpy(tmp->person.surname, p->surname, SURNAME_LEN);
    strncpy(tmp->person.email, p->email, EMAIL_LEN);
    strncpy(tmp->person.address, p->address, ADDRESS_LEN);

    tmp->prev = NULL;
    tmp->next = list->head;

    if(list->head)
        list->head->prev = tmp;

    list->head = tmp;
    return 0;
}

void DLL_remove_elem(DLL_List_t *list, DLL_Node_t *node){
    if(list->head == node)
        list->head = node->next;

    if(node->next)
        node->next->prev = node->prev;

    if(node->prev)
        node->prev->next = node->next;

    free(node);
}

int DLL_cmp_all(DLL_Person_t *p, DLL_Person_t *q){
    int i = strncmp(p->surname, q->surname, SURNAME_LEN);
    if(!i) {
        i = strncmp(p->name, q->name, NAME_LEN);
        if(!i){
            i = p->birth_date.year < q->birth_date.year ? -1 :
                p->birth_date.year == q->birth_date.year ? 0 : 1;
            if(!i) {
                i = p->birth_date.month < q->birth_date.month ? -1 :
                    p->birth_date.month == q->birth_date.month ? 0 : 1;
                if (!i) {
                    i = p->birth_date.day < q->birth_date.day ? -1 :
                        p->birth_date.day == q->birth_date.day ? 0 : 1;
                    if(!i){
                        i = strncmp(p->email, q->email, EMAIL_LEN);
                        if(!i){
                            i = p->phone < q->phone ? -1 :
                                p->phone == q->phone ? 0 : 1;
                            if(!i)
                                i = strncmp(p->address, q->address, ADDRESS_LEN);
                        }
                    }
                }
            }
        }
    }
    return i;
}

DLL_Person_t* DLL_create_person(void){
    return malloc(sizeof(DLL_Person_t));
}

int DLL_set_person(DLL_Person_t *p, char *name, char *surname,
                   uint8_t bday, uint8_t bmonth, uint16_t byear,
                   char *email, uint32_t phone, char *address)      {

    strncpy(p->name, name, NAME_LEN);
    strncpy(p->surname, surname, SURNAME_LEN);

    if(byear >= START_YEAR && byear <= END_YEAR)
        p->birth_date.year = byear;
    else
        return -1;

    if(bmonth >= 1 && bmonth <= 12)
        p->birth_date.month = bmonth;
    else
        return -1;

    int days[] = {31, byear % 4 == 0 ?
                      (byear % 100 == 0 ? (byear % 400 == 0 ? 29 : 28) : 29) : 28,
                  31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    if(bday >= 1 && bday <= days[bmonth])
        p->birth_date.day = bday;
    else
        return -1;

    if(phone >= PHONE_LEAST)
        p->phone = phone;
    else
        return -1;

    strncpy(p->email, email, EMAIL_LEN);
    strncpy(p->address, address, ADDRESS_LEN);

    if(p->name[NAME_LEN-1] || p->surname[SURNAME_LEN-1] ||
            p->email[EMAIL_LEN-1] || p->address[ADDRESS_LEN-1])
        return -1;

    return 0;
}

void DLL_print_list(DLL_List_t *list){
    DLL_Node_t *tmp = list->head;
    int i = 0;
    while(tmp){
        ++i;
/*        printf("%d:\tName: %s,\tSur: %s,\tBirth: %d/%d/%d,\tEmail: %s,\tPhone: %d,\tAddress: %s\n",
                i, tmp->person.name, tmp->person.surname, tmp->person.birth_date.day,
                tmp->person.birth_date.month, tmp->person.birth_date.year,
                tmp->person.email, tmp->person.phone, tmp->person.address);
*/
        printf("%d:\tName: %s,\tSur: %s\n", i, tmp->person.name, tmp->person.surname);

        tmp = tmp->next;
    }
}

