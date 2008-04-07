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

void kprint_int(int n){
  if (n <= 9){
    cons_putchar('0' + n);
    return;
  }

  kprint_int(n / 10);

  cons_putchar('0' + (n % 10));
}

/*int kstrlen(const char* string){
  int i;
  
  for (i = 0; TRUE; i++){
    if (string[i] == '\0'){
      return i;
    }
  }
  }*/
