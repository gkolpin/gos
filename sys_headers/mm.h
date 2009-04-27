#ifndef _MM_H
#define _MM_H

#include "types.h"

typedef struct mem_map{
  struct mem_map *next;
  uint32 page_loc;
  uint32 pages;
  bool reserved;
} mem_map;

typedef struct PAGE {
  /* fields used in kmalloc/slab allocation */
  struct PAGE 		*next_slab; /* used for kmalloc / slab allocator */
  struct PAGE	       	*prev_slab;
} PAGE;

/* allocates the specified number of pages */
void * alloc_pages(uint32 no_pages);
/* frees the specified number of pages at given location */
void free_pages(void *start_address, uint32 no_pages);
/* initialize the memory manager */
void mm_init(mem_map*, uint32 *start_page_reserved, uint32 *no_pages_reserved);
/* returns a physical address for a PAGE struct */
uint32 get_phys_address_for_page(PAGE*);
/* returns the page struct for a physical address */
PAGE * get_page_struct(void*);

#endif
