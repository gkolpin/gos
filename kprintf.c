#include "kprintf.h"
#include "console.h"
#include "types.h"

void kprintf(const char* string){
  int i;
  
  for (i = 0; TRUE; i++){
    if (string[i] == '\0')
      return;
    cons_putchar(string[i]);
  }
}

/*int kstrlen(const char* string){
  int i;
  
  for (i = 0; TRUE; i++){
    if (string[i] == '\0'){
      return i;
    }
  }
  }*/
