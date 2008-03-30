#include "console.h"
#include "kprintf.h"
#include "types.h"
#include "keyboard.h"
#include "gos.h"
#include "8259_pic.h"
#include "intvect.h"
#include "int.h"
#include "portio.h"

PRIVATE void kern_init();

void kern_start(void){
  int i;
  char input[3] = {'\0', '\n', '\0'};

  kern_init();
  
  kprintf("hello world");

  /*while (TRUE){
    cons_putchar(getchar());
    }*/

  //cmd_int(33);
  //cmd_int(33);

  cmd_sti();

  while (1){
    cmd_hlt();
  }
}

PRIVATE void kern_init(){
  pic_init();
  cons_init();
  kbd_init();
  intvect_init();
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
