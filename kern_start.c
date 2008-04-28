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

PRIVATE void kern_init(uint32 extended_mem, gdt_entry*, uint32 gdt_size, uint32 tss_pos, void* tss);

PRIVATE void proc1();
PRIVATE void proc2();

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

PRIVATE void proc1(){
  int i = 0;
  while (1) {
    kprintf("this is proc1: ");
    kprint_int(i);
    kprintf("\n");
    i++;
  }
}

PRIVATE void proc2(){
  int i = 0;
  while (1) {
    kprintf("this is proc2: ");
    kprint_int(i);
    kprintf("\n");
    i++;
  }
}

PRIVATE void kern_init(uint32 extended_mem, gdt_entry* gdt, uint32 gdt_size, uint32 tss_pos, void* tss){
  pic_init();
  cons_init();
  kbd_init();
  intvect_init();
  mm_init(extended_mem);
  sched_init();
  prot_init(gdt, gdt_size, tss_pos, tss);
}

void kb_int(void){
  cons_putchar(getchar());
}
