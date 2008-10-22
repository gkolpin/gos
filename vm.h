#ifndef _VM_H
#define _VM_H

#include "types.h"

/* Using 4kb pages. */
#define PAGE_SIZE 4096

typedef enum user_type {
  SUPERVISOR,
  USER
} user_type;


void vm_init(uint32 start_reserved_kernel_page, uint32 no_reserved_kernel_pages);

void * vm_alloc_at(void * phys_addr, uint32 requested_loc, uint32 size, user_type);
//uint32 kern_heap_virt_addr_start();
void* kern_phys_to_virt(void* phys_addr);
/*void * vm_malloc(uint32 size, user_type);*/

#endif
