#ifndef _VM_H
#define _VM_H

#include "types.h"

/* Using 4kb pages. */
#define PAGE_SIZE 4096u
/* kernel heap starts at 3GB */
#define KERNEL_HEAP_START 0xC0000000

typedef enum user_type {
  SUPERVISOR,
  USER
} user_type;

/* number of pages to allocate for given number of bytes */
#define PAGES_FOR_BYTES(bytes) ((bytes / PAGE_SIZE) + (bytes % PAGE_SIZE ? 1 : 0))
/* page alignment */
#define PAGE_ALIGN_DOWN(addr) (addr & ~(PAGE_SIZE - 1))
#define PAGE_ALIGN_UP(addr) (PAGE_ALIGN_DOWN(addr) + (addr & (PAGE_SIZE - 1))\
			     * PAGE_SIZE)

void vm_init(uint32 start_reserved_kernel_page, uint32 no_reserved_kernel_pages);

void * vm_alloc_at(void * phys_addr, uint32 requested_loc, uint32 size, user_type);
//uint32 kern_heap_virt_addr_start();
void * kern_phys_to_virt(void* phys_addr);
void * kern_virt_to_phys(void* virt_addr);
void * kmemmap2virt(void *phys_addr, uint32 no_pages);
void * virt2phys(uint32 pd_phys_addr, void* virt_src);
/* returns the physical address of the new (copied) page directory */
uint32 copy_cur_page_dir();
void pd_free(uint32 pd_phys_addr);
void set_pd(uint32 pd_phys);

/*void * vm_malloc(uint32 size, user_type);*/

#endif
