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
#include "sched.h"
#include "mm.h"
#include "testproc.h"

PRIVATE void kern_init(uint32 extended_mem, gdt_entry*, uint32 gdt_size, uint32 tss_pos, void* tss);

void kern_start(uint32 extended_mem, void* gdt, uint32 gdt_size, uint32 tss_pos, void* tss){
  int i;
  char input[3] = {'\0', '\n', '\0'};

  kern_init(extended_mem, gdt, gdt_size, tss_pos, tss);
  
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

  task * t1 = create_task(proc1);
  task * t2 = create_task(proc2);
  schedule(t1);
  schedule(t2);

  sched_int();

  //cmd_sti();

  while (1){
    cmd_hlt();
    //sleep(1000);
    //kprintf("awoken\n");
  }
}

PRIVATE void kern_init(uint32 extended_mem, gdt_entry* gdt, uint32 gdt_size, uint32 tss_pos, void* tss){
  pic_init();
  kprintf("pic_initted\n");
  cons_init();
  kprintf("cons_initted\n");
  kbd_init();
  kprintf("kbd_initted\n");
  intvect_init();
  kprintf("intvect_initted\n");
  mm_init(extended_mem);
  kprintf("mm_initted\n");
  sched_init();
  kprintf("sched_initted\n");
  prot_init(gdt, gdt_size, tss_pos, tss);
  kprintf("prot_initted\n");
}

void kb_int(void){
  cons_putchar(getchar());
}
