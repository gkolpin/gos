#include "console.h"
#include "kprintf.h"

void kern_start(void){
  int i;
  char input[3] = {'\0', '\n', '\0'};
  cons_init();
  
  for (i = 'a'; i <= 'z'; i++){
    input[0] = (char)i;
    kprintf(input);
  }

  while (1);
}
