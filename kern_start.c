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
#include "at_disk_driver.h"
#include "simple_fs.h"
#include "vm.h"

PRIVATE void kern_init(uint32 extended_mem, gdt_entry*, uint32 gdt_size, uint32 tss_pos, void* tss);

void kern_start(uint32 extended_mem, void* gdt, uint32 gdt_size, uint32 tss_pos, void* tss){
  int i;
  char input[3] = {'\0', '\n', '\0'};
  uint8 hd_input_buf[512];
  byte_t *prog1buf;

  kern_init(extended_mem, gdt, gdt_size, tss_pos, tss);
  
  /*kprint_int(extended_mem);
  kprintf("\n");
  kprint_int((uint32)gdt);
  kprintf("\n");
  kprint_int(gdt_size);
  kprintf("\n");
  kprint_int(tss_pos);
  kprintf("\n");*/

  /*kprintf("FS_INDEX: ");
  disk_read_sector(1000, hd_input_buf);
  kprint_int(hd_input_buf[0]);*/

  /*while (TRUE){
    cons_putchar(getchar());
    }*/

  //cmd_int(33);
  //cmd_int(33);

  /*  task * t1 = create_task(proc1);
  task * t2 = create_task(proc2);
  schedule(t1);
  schedule(t2);*/

  kprintf("begin load program\n");
  //prog1buf = (byte_t*)malloc(get_file_size(0));
  /* vm map program to 0x100000 (1MB) */
  prog1buf = (byte_t*)vm_malloc(0x200000, get_file_size(0), USER);

  kprintf("%x\n", (uint32)prog1buf);
  load_file(0, prog1buf);
  kprintf("program loaded...\n");
  kprintf("%x\n", (uint32)prog1buf);
  task * t1 = create_task(prog1buf);
  schedule(t1);

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
  vm_init();
  kprintf("vm_initted\n");
  sched_init();
  kprintf("sched_initted\n");
  prot_init(gdt, gdt_size, tss_pos, tss);
  kprintf("prot_initted\n");
  hd_driver_init();
}

void kb_int(void){
  cons_putchar(getchar());
}
