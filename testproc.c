#include "gos.h"
#include "types.h"
#include "syscall.h"

PRIVATE void _printf(const char* string);
PRIVATE void _print_int(int n);
PRIVATE void _cons_putchar(char c);
PRIVATE int _fork();
PRIVATE int _kill();
PRIVATE uint32 _uptime();
PRIVATE void _exit(int status);
PRIVATE void _waitpid(uint32 pid, uint32 *status, uint32 opts);
PRIVATE void _wait(uint32 *status);
PRIVATE int _brk(const void *addr);
PRIVATE void * _sbrk(int incr);
void proc2();

void proc1(){
  int i = 0;
  int proc_id;
  uint32 status;
  char *heap;

  proc_id = _fork();
  
  if (proc_id == 0)
    proc2();

  _waitpid(proc_id, &status, 0);

  _print_int(status);
  _printf("\n");
  heap = _sbrk(1);
  heap[0] = 5;
  _printf("\n");
  _print_int((uint32)heap);

  while (1) {
    _printf("this is proc1: ");
    _print_int(i);
    _printf("\n");
    i++;
  }
}

PRIVATE uint32 _make_syscall(uint32 p1, uint32 p2, uint32 p3, uint32 p4){
  return MAKE_SYSCALL(p1, p2, p3, p4);
}

PRIVATE void _cons_putchar(char c){
  _make_syscall(CONS_PUTCHAR, c, 0, 0);
}

PRIVATE int _fork(){
  return _make_syscall(FORK, 0, 0, 0);
}

PRIVATE int _kill(uint32 pid){
  return _make_syscall(KILL, pid, 0, 0);
}

PRIVATE uint32 _uptime(){
  return _make_syscall(UPTIME, 0, 0, 0);
}

PRIVATE void _exit(int status){
  _make_syscall(EXIT, status, 0, 0);
}
 
PRIVATE void _waitpid(uint32 pid, uint32 *status, uint32 opts){
  _make_syscall(WAITPID, pid, (uint32)status, opts);
}

PRIVATE void _wait(uint32 *status){
  /* -1: wait for any child process */
  _waitpid(-1, status, 0);
}

PRIVATE int _brk(const void *addr){
  return _make_syscall(BRK, (uint32)addr, 0, 0);
}

PRIVATE void * _sbrk(int incr){
  return (void*)_make_syscall(SBRK, incr, 0, 0);
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

    if (i == 500)
      _exit(44);

    i++;
  }
}
