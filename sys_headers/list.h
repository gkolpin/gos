#ifndef _LIST_H
#define _LIST_H

typedef struct _list *list;

typedef struct list_node {
  struct list_node *prev, *next;
} list_node;

#include "types.h"

#define cur_obj(lst, l_node_p, type) ((type*)((uintptr_t)(l_node_p) - list_node_offset(lst)))
#define list_init(type, member) (_list_init((size_t)&(((type*)0)->member)))

list _list_init(size_t node_offset);
bool destroy_list(list);
void list_add(list l, list_node *node);
void list_remove(list l, list_node *node);
list_node * list_next(list l, list_node*);
list_node * list_head(list l);
size_t list_node_offset(list l);
bool list_empty(list);
void list_insert_before(list_node *to_insert, list_node *node_in_list);
void list_prepend(list l, list_node *node);
int list_length(list);

#endif
