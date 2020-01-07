#include "list.h"
#include <stdlib.h>

void list_init(list *l)
{
    l->next = NULL;
    l->prec = NULL;
}

int list_is_empty(list *l)
{
    return l->next == NULL;
}

size_t list_len(list *l)
{
    list *cur = l->next;
    if(cur == NULL) {
        return 0;
    }
    size_t length = 1;
    while (cur->next != NULL)
    {
        length++;
        cur = cur->next;
    }
    return length;
}

void list_push_front(list *l, list *elm)
{
    list *old = l->next;
    l->next = elm;
    elm->next = old;
    if (l->prec == NULL)
        l->prec = elm;
    if (old != NULL)
        old->prec = elm;
    elm->prec = l;
}

void list_push_tail(list *l, list *elm)
{
    list *cur = l->prec;
    if (cur != NULL) {
        cur->next = elm;
        elm->prec = cur;
    }
    else {
        l->next = elm;
    }
    l->prec = elm;
}

list *list_pop_front(list *l)
{
    list *res = l->next;
    if (res == NULL) {
        return NULL;
    }
    if (res->next == NULL) {
        l->prec = NULL;
    }
    else {
        res->next->prec = l;
    }
    l->next = res->next;

    return res;
}

list *list_pop_tail(list *l)
{
    list *res = l->prec;
    if (l->next == res) {
        l->prec = NULL;
        l->next = NULL;
    }
    else {
        l->prec = res->prec;
        res->prec->next = NULL;
    }

    return res;
}

