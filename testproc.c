#include "gos.h"
#include "types.h"
#include "syscall.h"

PRIVATE void _printf(const char* string);
PRIVATE void _print_int(int n);
PRIVATE void _cons_putchar(char c);
PRIVATE int _fork();
void proc2();

void proc1(){
  int i = 0;
  int proc_id;

  proc_id = _fork();
  
  if (proc_id != 0)
    proc2();

  while (1) {
    _printf("this is proc1: ");
    _print_int(i);
    _printf("\n");
    i++;
  }
}

PRIVATE uint32 _make_syscall(uint32 p1, uint32 p2, uint32 p3, uint32 p4){
  MAKE_SYSCALL(p1, p2, p3, p4);
}

PRIVATE void _cons_putchar(char c){
  _make_syscall(CONS_PUTCHAR, c, 0, 0);
}

PRIVATE int _fork(){
  _make_syscall(FORK, 0, 0, 0);
}

PRIVATE int _kill(uint32 pid){
  _make_syscall(KILL, pid, 0, 0);
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
