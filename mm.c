#include "gos.h"
#include "types.h"
#include "mm.h"

typedef struct mem_list_node {
  struct mem_list_node*	next;
  uint32 		amnt;
  bool 			free;
  void*			location;
} mem_list_node;

PRIVATE mem_list_node mem;

void * malloc(uint32 amnt){
  mem_list_node *cur_node;
  mem_list_node *new_node;
  for (cur_node = &mem; cur_node != NULL; cur_node = cur_node->next){
    if (cur_node->free && cur_node->amnt >= amnt){
      /* modify cur_node so that it is not free
	 and so that the remaining free space is a new
	 free mem node */
      cur_node->free = FALSE;
      /* create new new free mem node if there's enough memory */
      if ((cur_node->amnt - amnt - sizeof(mem_list_node)) > 0){
	new_node = (void*)(cur_node->location + amnt);
	new_node->next = cur_node->next;
	new_node->amnt = cur_node->amnt - amnt - sizeof(mem_list_node);
	new_node->free = TRUE;
	new_node->location = (void*)(new_node + sizeof(mem_list_node));

	cur_node->next = new_node;
      } 
      
      return cur_node->location;
    }
  }
  
  return NULL;
}

void mm_init(uint32 blocks){
  mem.next = NULL;
  mem.amnt = blocks * 1024;
  mem.free = TRUE;
  mem.location = (void*)0x100000; /* start memory at 1MB */
}
