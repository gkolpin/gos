#ifndef _PROT_H
#define _PROT_H

#include "types.h"

typedef struct gdt_entry{
  uint32 dword1;
  uint32 dword2;
} gdt_entry;

void prot_init(gdt_entry*, uint32 gdt_size, uint32 tss_pos, void* tss);

#endif
