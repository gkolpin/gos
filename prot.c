#include "prot.h"
#include "types.h"
#include "gos.h"
#include "kprintf.h"

PRIVATE void init_tss_desc(gdt_entry*, void* tss, uint32 tss_pos);

void prot_init(gdt_entry *gdt, uint32 gdt_size, uint32 tss_pos, void* tss){
  init_tss_desc(&gdt[tss_pos], tss, tss_pos);
}

PRIVATE void init_tss_desc(gdt_entry *tss_desc, void* tss, uint32 tss_pos){
  uint32 tss_addr = (uint32)tss_desc;
  uint16 *dword1 = (uint16*)&tss_desc->dword1;
  uint8 *dword2 = (uint8*)&tss_desc->dword2;

  dword1[0] = 0x67;		/* limit */
  dword1[1] = tss_addr & 0xFFFF; 	/* 0:15 of base address */

  dword2[0] = (tss_addr >> 16) & 0xFF; /* 16:23 of base */
  dword2[1] = 0x89;
  dword2[2] = 0x10;
  dword2[3] = tss_addr >> 24;	/* 24:31 of base */

  /* load tss register with tss segment selector */
  cmd_ltr((uint16)(tss_pos << 3));
}
