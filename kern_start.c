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

PRIVATE void _make_syscall(uint32 p1, uint32 p2, uint32 p3, uint32 p4){
  MAKE_SYSCALL(p1, p2, p3, p4);
}

PRIVATE void _cons_putchar(char c){
  _make_syscall(0, c, 0, 0);
}

PRIVATE void _printf(const char* string){
  int i;
  
  for (i = 0; TRUE; i++){
    if (string[i] == '\0')
      return;
    _cons_putchar(string[i]);
  }
}

PRIVATE void _print_int(int n){
  if (n <= 9){
    _cons_putchar('0' + n);
    return;
  }

  _print_int(n / 10);

  _cons_putchar('0' + (n % 10));
}

PRIVATE void proc1(){
  int i = 0;
  while (1) {
    _printf("this is proc1: ");
    _print_int(i);
    _printf("\n");
    i++;
  }
}

PRIVATE void proc2(){
  int i = 0;
  while (1) {
    _printf("this is proc2: ");
    _print_int(i);
    _printf("\n");
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
