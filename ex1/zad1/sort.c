#include <stddef.h>
#include <stdio.h>
#include "list.h"

static DLL_Node_t* split(DLL_Node_t *head){
    if(!head || !head->next)
        return NULL;

    DLL_Node_t *jumper = head;
    while(jumper->next && jumper->next->next){
        head = head->next;
        jumper = jumper->next->next;
    }

    jumper = head->next;
    head->next = NULL;
    return jumper;
}

static DLL_Node_t* merge(DLL_Node_t *A, DLL_Node_t *B, DLL_cmp_t cmp){
    if(!A)
        return B;
    if(!B)
        return A;

    DLL_Node_t *head;
    DLL_Node_t *tail;

    if(cmp(&A->person, &B->person) <= 0) {
        head = tail = A;
        A = A->next;
    }
    else {
        head = tail = B;
        B = B->next;
    }

    while(A && B){
        if(cmp(&A->person, &B->person) <= 0) {
            tail->next = A;
            A = A->next;
        }
        else {
            tail->next = B;
            B = B->next;
        }

        tail->next->prev = tail;
        tail = tail->next;
    }

    if(A)
        tail->next = A;
    if(B)
        tail->next = B;

    tail->next->prev = tail;
    return head;
}

static DLL_Node_t* mergesort(DLL_Node_t *list, DLL_cmp_t cmp){
    if(!list)
        return NULL;
    if(!list->next)
        return list;

    DLL_Node_t *mid = split(list);
    return merge(mergesort(list, cmp), mergesort(mid, cmp), cmp);
}

void DLL_sort(DLL_List_t *list, DLL_cmp_t cmp){
    list->head = mergesort(list->head, cmp);
}
