#ifndef _UTILITY_H
#define _UTILITY_H

#include "types.h"

void * kmemcpy(void *dest, const void *src, uint32 n);
void * kmemcpy2phys(void *phys_dest, const void *virt_src, uint32 size);
void kmemcpyphys(uint32 page_dest, uint32 page_src, uint32 no_pages);
void bzero(void*, uint32);
uint32 get_eflags();
uint32 bit_scan_forward(uint32);

#endif
