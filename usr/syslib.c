#include "syslib.h"
#include "syscalls.h"

#define PRIVATE static

extern int MAKE_SYSCALL(uint32 p1, uint32 p2, uint32 p3, uint32 p4);

PRIVATE uint32 make_syscall(uint32 p1, uint32 p2, uint32 p3, uint32 p4){
  return MAKE_SYSCALL(p1, p2, p3, p4);
}

void cons_putchar(char c){
  make_syscall(CONS_PUTCHAR, c, 0, 0);
}

int fork(){
  return make_syscall(FORK, 0, 0, 0);
}

int kill(uint32 pid){
  return make_syscall(KILL, pid, 0, 0);
}

uint32 uptime(){
  return make_syscall(UPTIME, 0, 0, 0);
}

void exit(int status){
  make_syscall(EXIT, status, 0, 0);
}
 
void waitpid(uint32 pid, uint32 *status, uint32 opts){
  make_syscall(WAITPID, pid, (uint32)status, opts);
}

void wait(uint32 *status){
  /* -1: wait for any child process */
  waitpid(-1, status, 0);
}

int brk(const void *addr){
  return make_syscall(BRK, (uint32)addr, 0, 0);
}

void * sbrk(int incr){
  return (void*)make_syscall(SBRK, incr, 0, 0);
}

int getpid(){
  return make_syscall(GETPID, 0, 0, 0);
}
