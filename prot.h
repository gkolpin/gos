#ifndef _PROT_H
#define _PROT_H

#include "types.h"

#define R0_CODE_S	0x8
#define R0_DATA_S	0x10
#define R3_CODE_S	0x28
#define R3_DATA_S	0x20

typedef struct gdt_entry{
  uint32 dword1;
  uint32 dword2;
} gdt_entry;

void prot_init(gdt_entry*, uint32 gdt_size, uint32 tss_pos, void* tss);

#endif
