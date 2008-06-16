#include "gos.h"
#include "kprintf.h"
#include "console.h"
#include "types.h"

PRIVATE void kprint_int_recurse(int);

void kprintf(const char* string){
  int i;
  
  for (i = 0; TRUE; i++){
    if (string[i] == '\0')
      return;
    cons_putchar(string[i]);
  }
}

void kprint_int(int n){
  kprint_int_recurse(n);
  kprintf("\n");
}

PRIVATE void kprint_int_recurse(int n){
  if (n <= 9){
    cons_putchar('0' + n);
    return;
  }

  kprint_int_recurse(n / 10);

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
