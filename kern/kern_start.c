#include "console.h"
#include "rs232.h"
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
#include "at_disk_driver.h"
#include "simple_fs.h"
#include "vm.h"
#include "kmalloc.h"
#include "elf_loader.h"
#include "clock.h"
#include "devices.h"
#include "vfs.h"
#include "devfs.h"

PRIVATE void kern_init(uint32 extended_mem, gdt_entry*, uint32 gdt_size, uint32 tss_pos, void* tss);
PRIVATE void retrieve_mem_map(uint32 extended_mem, mem_map*);

void kern_start(uint32 extended_mem, void* gdt, uint32 gdt_size, uint32 tss_pos, void* tss){
  /*int i;*/
  /*char input[3] = {'\0', '\n', '\0'};*/
  /*uint8 hd_input_buf[512];*/
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

  /*cmd_int(33);
    cmd_int(33);*/

  /*  task * t1 = create_task(proc1);
  task * t2 = create_task(proc2);
  schedule(t1);
  schedule(t2);*/

  kprintf("begin load program\n");
  /*prog1buf = (byte_t*)malloc(get_file_size(0));*/
  /* vm map program to 0x100000 (1MB) */
  uint32 prog_size = PAGES_FOR_BYTES(get_file_size(0));
  kprintf("file size in bytes: %d\n", get_file_size(0));
  kprintf("num file pages: %d\n", prog_size);
  /*prog1buf = (byte_t*)kmalloc(prog_size * 4096);*/
  prog1buf = kmalloc(prog_size * PAGE_SIZE);
  kprintf("allocated pages for task\n");

  kprintf("prog1buf: %x\n", (uint32)prog1buf);
  kprintf("before load_file\n");
  load_file(0, prog1buf);
  kprintf("program loaded...\n");
  kprintf("%x\n", (uint32)prog1buf);

  task * t1 = create_task_from_elf(prog1buf, prog_size * PAGE_SIZE);
  kprintf("created task\n");
  kfree(prog1buf);
  kprintf("freed prog1buf\n");

  kprintf("created task from elf\n");
  schedule(t1);
  kprintf("scheduled\n");

  /*sched_int();*/
  restart_task();

  /*cmd_sti();*/

  while (1){
    cmd_hlt();
    /*sleep(1000);
      kprintf("awoken\n");*/
  }
}

/*
PRIVATE void test_kmalloc(){
  void *t1, *t2, *t3;
  t1 = kmalloc(32);
  t2 = kmalloc(32);

  kprintf("t1: %d, t2: %d\n", (uint32)t1, (uint32)t2);

  if ((uint32)t2 - (uint32)t1 == 32){
    kprintf("difference is 32\n");
  }

  kfree(t1);
  t3 = kmalloc(32);
  if (t3 == t1){
    kprintf("t1 is same as t3\n");
  }

  kfree(t2);
  t3 = kmalloc(32);
  if (t3 == t2){
    kprintf("t3 is the same as t2\n");
  }

  t1 = kmalloc(PAGE_SIZE);
  kprintf("kmalloc1 complete\n");
  t2 = kmalloc(PAGE_SIZE);
  kprintf("kmalloc2 complete\n");

  ((uint8*)t1)[0] = 'a';
  ((uint8*)t2)[0] = 'a';

  kprintf("t1,: %u, t2: %u\n", (uint32)t1, (uint32)t2);
}
*/

PRIVATE void kern_init(uint32 extended_mem, gdt_entry* gdt, uint32 gdt_size, uint32 tss_pos, void* tss){
  uint32 mm_start_page_reserved;
  uint32 mm_no_pages_reserved;
  mem_map map[128];

  pic_init();
  serial_init();
  cons_init();
  kprintf("cons_initted\n");
  intvect_init();
  kprintf("intvect_initted\n");
  retrieve_mem_map(extended_mem, map);
  mm_init(map,
	  &mm_start_page_reserved, &mm_no_pages_reserved);
  kprintf("mm_initted\n");
  vm_init(mm_start_page_reserved, mm_no_pages_reserved);
  kprintf("vm_initted\n");
  kmalloc_init();
  kprintf("kmalloc_initted\n");
  sched_init();
  kprintf("sched_initted\n");
  prot_init(gdt, gdt_size, tss_pos, tss);
  kprintf("prot_initted\n");
  clock_init();
  kprintf("clock initted\n");
  hd_driver_init();
  kprintf("hd_driver initted\n");
  vfs_init();
  kprintf("vfs initted\n");
  devfs_init();
  kprintf("devfs initted\n");
  init_devices();
  kprintf("devices initted\n");
  vfs_mount("devfs", "/dev");
  kprintf("devfs mounted\n");
}

PRIVATE void retrieve_mem_map(uint32 extended_mem, mem_map *map){
  map[0].next = &map[1];
  map[0].page_loc = 0;
  /* first MB of mem reserved for kernel */
  map[0].pages = 0x100000 / PAGE_SIZE;
  map[0].reserved = TRUE;

  map[1].next = NULL;
  map[1].page_loc = 0x100000 / PAGE_SIZE;
  map[1].pages = extended_mem * 0x400 /* 1024 */ / PAGE_SIZE;
  map[1].reserved = FALSE;
}
