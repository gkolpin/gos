#include "gos.h"
#include "types.h"

PRIVATE void _printf(const char* string);
PRIVATE void _print_int(int n);

void proc1(){
  int i = 0;
  while (1) {
    _printf("this is proc1: ");
    _print_int(i);
    _printf("\n");
    i++;
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

void proc2(){
  int i = 0;
  while (1) {
    _printf("this is proc2: ");
    _print_int(i);
    _printf("\n");
    i++;
  }
}
