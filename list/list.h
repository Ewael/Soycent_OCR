#ifndef LIST_H
#define LIST_H
#include <stdlib.h>

typedef struct list {
    void *data;
    struct list *next;
    struct list *prec;
}list;

void list_init(list *l);

int list_is_empty(list *l);

size_t list_len(list *l);

void list_push_front(list *l, list *elm);

void list_push_tail(list *l, list *elm);

list *list_pop_front(list *l);

list *list_pop_tail(list *l);

#endif
