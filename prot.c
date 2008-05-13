#include "prot.h"
#include "types.h"
#include "gos.h"
#include "kprintf.h"

#define ESP_0 	1
#define SS_0	2

extern uint32 GOS_BOTTOM_STACK;

PRIVATE void init_tss_desc(gdt_entry*, void* tss, uint32 tss_pos);
void cmd_ltr(uint16 tss_pos);

void prot_init(gdt_entry *gdt, uint32 gdt_size, uint32 tss_pos, void* tss){
  tss_p = tss;
  init_tss_desc(&gdt[tss_pos], tss, tss_pos);
}

PRIVATE void init_tss_desc(gdt_entry *tss_desc, void* tss, uint32 tss_pos){
  //uint32 tss_addr = (uint32)tss_desc;
  uint32 tss_addr = (uint32)tss;
  uint16 *dword1 = (uint16*)&tss_desc->dwords[0];
  uint8 *dword2 = (uint8*)&tss_desc->dwords[1];

  dword1[0] = 0x67;		/* limit */
  dword1[1] = tss_addr & 0xFFFF; 	/* 0:15 of base address */

  dword2[0] = (tss_addr >> 16) & 0xFF; /* 16:23 of base */
  dword2[1] = 0x89;
  dword2[2] = 0x10;
  dword2[3] = tss_addr >> 24;	/* 24:31 of base */

  /* load kernel's stack info into tss */
  ((uint32*)tss)[SS_0] = R0_DATA_S;
  ((uint32*)tss)[ESP_0] = (uint32)&GOS_BOTTOM_STACK;

  /* load tss register with tss segment selector */
  cmd_ltr((uint16)(tss_pos << 3));
}
