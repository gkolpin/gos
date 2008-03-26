#include "console.h"
#include "kprintf.h"
#include "types.h"
#include "keyboard.h"
#include "gos.h"
#include "8259_pic.h"

PRIVATE void kern_init();

void kern_start(void){
  int i;
  char input[3] = {'\0', '\n', '\0'};

  kern_init();
  
  kprintf("hello world");

  while (TRUE){
    cons_putchar(getchar());
  }

  while (1);
}

PRIVATE void kern_init(){
  pic_init();
  cons_init();
  kbd_init();
}
