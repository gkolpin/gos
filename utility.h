#ifndef _UTILITY_H
#define _UTILITY_H

#include "types.h"

void * kmemcpy(void *dest, const void *src, uint32 n);
uint32 get_eflags();
uint32 bit_scan_forward(uint32);

#endif
