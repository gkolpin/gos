#include "console.h"
#include "kprintf.h"
#include "types.h"
#include "keyboard.h"
#include "gos.h"
#include "8259_pic.h"
#include "intvect.h"
#include "int.h"
#include "portio.h"
#include "ksignal.h"
#include "prot.h"

PRIVATE void kern_init(gdt_entry*, uint32 gdt_size, uint32 tss_pos, void* tss);

void kern_start(uint32 extended_mem, void* gdt, uint32 gdt_size, uint32 tss_pos, void* tss){
  int i;
  char input[3] = {'\0', '\n', '\0'};

  kern_init(gdt, gdt_size, tss_pos, tss);
  
  kprint_int(extended_mem);
  kprintf("\n");
  kprint_int((uint32)gdt);
  kprintf("\n");
  kprint_int(gdt_size);
  kprintf("\n");
  kprint_int(tss_pos);

  /*while (TRUE){
    cons_putchar(getchar());
    }*/

  //cmd_int(33);
  //cmd_int(33);

  cmd_sti();

  while (1){
    cmd_hlt();
    //sleep(1000);
    //kprintf("awoken\n");
  }
}

PRIVATE void kern_init(gdt_entry* gdt, uint32 gdt_size, uint32 tss_pos, void* tss){
  pic_init();
  cons_init();
  kbd_init();
  intvect_init();
  prot_init(gdt, gdt_size, tss_pos, tss);
}

void kb_int(void){
  cons_putchar(getchar());

  /* eoi to keyboard */
  //outb(0x20, 0x61);
  /* eoi to controller 2 */
  outb(0xA0, 0x20);
  /* eoi to controller 1 */
  outb(0x20, 0x20);
}
