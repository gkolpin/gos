#include "gos.h"
#include "types.h"
#include "mm.h"

#define NO_MEM_NODES 100
#define MEM_PAGE_SIZE 4096
#define MEM_START_ADDR 0x100000
#define BLOCK_SIZE 1024

#define ERROR -1

typedef struct mem_list_node {
  struct mem_list_node*	next;
  uint32		start;
  uint32 		amnt;
  bool 			free;
  bool			in_use;
} mem_list_node;

PRIVATE mem_list_node node_slots[NO_MEM_NODES];
PRIVATE uint32 slots_used;

PRIVATE uint32 get_addr_of_page(uint32 page_no);
PRIVATE int allocate_pages(int required_pages);
PRIVATE void set_node(mem_list_node *node, mem_list_node *next, uint32 start, 
		      uint32 amnt, bool free, bool in_use);
PRIVATE uint32 bytes_to_pages(uint32 bytes);
PRIVATE int find_slot_of_size(uint32 pages);
PRIVATE int find_unused_slot();


/* amnt is in bytes */
void * malloc(uint32 amnt){
  int slot_no;
  uint32 start_addr;
  uint32 required_pages = bytes_to_pages(amnt);
  slot_no = allocate_pages(required_pages);
  start_addr = get_addr_of_page(node_slots[slot_no].start);
  return (void*)start_addr;
}

PRIVATE uint32 get_addr_of_page(uint32 page_no){
  return page_no * MEM_PAGE_SIZE + MEM_START_ADDR;
}

PRIVATE int allocate_pages(int required_pages){
  int slot_no = find_slot_of_size(required_pages);
  mem_list_node *slot = &node_slots[slot_no];
  int unused_slot_no;
  mem_list_node *unused_slot;

  if (slot->amnt != required_pages &&
      slots_used == NO_MEM_NODES){
    /* aren't enough slots enough to split this node, so return error */
    return ERROR;
  }

  if (slot->amnt != required_pages){
    /* split needed */
    unused_slot_no = find_unused_slot();
    unused_slot = &node_slots[unused_slot_no];
    /* create new node */
    set_node(unused_slot, NULL, slot->start + required_pages, 
	     slot->amnt - required_pages, TRUE, TRUE);
  }

  /* change values in original node */
  set_node(slot, unused_slot, slot->start, 
	   required_pages, FALSE, TRUE);

  return slot_no;
}

PRIVATE int find_unused_slot(){
  int i;
  for (i = 0; i < NO_MEM_NODES; i++){
    if (!node_slots[i].in_use){
      return i;
    }
  }

  return ERROR;
}

PRIVATE void set_node(mem_list_node *node, mem_list_node *next, uint32 start, 
		      uint32 amnt, bool free, bool in_use){
  node->next = next;
  node->start = start;
  node->amnt = amnt;
  node->free = free;
  node->in_use = in_use;
}

PRIVATE uint32 bytes_to_pages(uint32 bytes){
  bool mul_of_page_sz;
  int base_pages = 0;
  mul_of_page_sz = bytes % MEM_PAGE_SIZE == 0 ? TRUE : FALSE;
  base_pages = bytes / MEM_PAGE_SIZE;
  base_pages += mul_of_page_sz ? 0 : 1;
  return base_pages;  
}

PRIVATE int find_slot_of_size(uint32 pages){
  int i;
  for (i = 0; i < NO_MEM_NODES; i++){
    if (!node_slots[i].in_use &&
	node_slots[i].free &&
	node_slots[i].amnt >= pages){
	
      return i;
    }
  }
  
  return ERROR;
}

void mm_init(uint32 blocks){
  int i;
  /* initialize all node slots to unused */
  for (i = 0; i < NO_MEM_NODES; i++){
    node_slots[i].in_use = FALSE;
  }

  /* initialize first node with free memory */

  node_slots[0].next = NULL;
  node_slots[0].start = 0;
  node_slots[0].amnt = blocks * BLOCK_SIZE;
  node_slots[0].free = TRUE;
  node_slots[0].in_use = TRUE;
}
