#include "list.h"
#include "gos.h"
#include "types.h"
#include "kmalloc.h"

struct _list {
  list_node head;
  size_t node_offset;
};

list _list_init(size_t node_offset){
  list retList = (list)kmalloc(sizeof(struct _list));
  retList->head.prev = retList->head.next = &retList->head;
  retList->node_offset = node_offset;
  return retList;
}

bool destroy_list(list l){
  if (list_empty(l)){
    kfree(l);
    return TRUE;
  }
  return FALSE;
}

void list_add(list l, list_node *node){
  node->prev = l->head.prev;
  node->next = &(l->head);

  node->prev->next = node;
  l->head.prev = node;
}

void list_remove(list l, list_node *node){
  node->prev->next = node->next;
  node->next->prev = node->prev;
}

list_node * list_next(list l, list_node *node){
  if (&l->head == node->next){
    return NULL;
  }

  return node->next;
}

list_node * list_head(list l){
  if (list_empty(l)) return NULL;
  return l->head.next;
}

size_t list_node_offset(list l){
  return l->node_offset;
}

bool list_empty(list l){
  return l->head.next == &l->head;
}

void list_insert_before(list_node *to_insert, list_node *node_in_list){
  if (NULL == node_in_list)
    return;

  to_insert->next = node_in_list;
  to_insert->prev = node_in_list->prev;
  node_in_list->prev = to_insert;
  node_in_list->prev = to_insert;
}

void list_prepend(list l, list_node *node){
  list_node *l_node = list_head(l);
  if (NULL == l_node){
    list_add(l, node);
  } else {
    list_insert_before(node, l_node);
  }
}
