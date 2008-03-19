#include "console.h"
#include "kprintf.h"

void kern_start(void){
  cons_init();
  kprintf("hello \n world \n this is working \n now");
  while (1);
}
