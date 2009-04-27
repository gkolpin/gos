#include "gos.h"
#include "types.h"
#include "syslib.h"

PRIVATE void _printf(const char* string);
PRIVATE void _print_int(int n);

void proc2();

void proc1(){
  int i = 0;
  int proc_id;
  uint32 status;
  char *heap;

  proc_id = fork();
  
  if (proc_id == 0)
    proc2();

  waitpid(proc_id, &status, 0);

  _print_int(status);
  _printf("\n");
  heap = sbrk(1);
  heap[0] = 5;
  _printf("\n");
  _print_int((uint32)heap);

  while (1) {
    _printf("this is proc ");
    _print_int(getpid());
    _print_int(i);
    _printf("\n");
    i++;
  }
}

PRIVATE void _printf(const char* string){
  int i;
  
  for (i = 0; TRUE; i++){
    if (string[i] == '\0')
      return;
    cons_putchar(string[i]);
  }
}

PRIVATE void _print_int(int n){
  if (n <= 9){
    cons_putchar('0' + n);
    return;
  }

  _print_int(n / 10);

  cons_putchar('0' + (n % 10));
}

void proc2(){
  int i = 0;
  while (1) {
    _printf("this is proc ");
    _print_int(getpid());
    _print_int(i);
    _printf("\n");

    if (i == 500)
      exit(44);

    i++;
  }
}
