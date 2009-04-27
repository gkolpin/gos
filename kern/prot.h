#ifndef _PROT_H
#define _PROT_H

#include "types.h"

/* ring 0 selectors */
#define R0_CODE_S	0x8
#define R0_DATA_S	0x10
/* ring 3 selectors */
/* set RPL to 3 */
#define R3_CODE_S	0x2B
#define R3_DATA_S	0x23

void *tss_p;

typedef qword_t gdt_entry;
typedef qword_t descriptor;

void prot_init(gdt_entry*, uint32 gdt_size, uint32 tss_pos, void* tss);

#endif
