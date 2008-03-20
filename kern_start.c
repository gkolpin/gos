#include "console.h"
#include "kprintf.h"
#include "types.h"

void kern_start(void){
  int i;
  char input[3] = {'\0', '\n', '\0'};
  cons_init();
  
  while (TRUE){
    for (i = 'a'; i <= 'z'; i++){
      input[0] = (char)i;
      kprintf(input);
    }
  }

  while (1);
}
