#include "syscall.h"

void ksyscall(uint32 p1, uint32 p2, uint32 p3, uint32 p4){
  /* p1 is syscall number */
  switch(p1){
  case CONS_PUTCHAR:
    cons_putchar((uint8)p2);
    break;
  case FORK:
    fork_cur_task();
    break;
  case KILL:
    kill_task(p2);
    break;
  case UPTIME:
    get_uptime();
    break;
  case EXIT:
    exit_cur_task(p2);
    break;
  case WAITPID:
    waitpid(p2, p3, p4);
    break;
  case BRK:
    brk((void*)p2);
    break;
  case SBRK:
    sbrk(p2);
    break;
  case GETPID:
    getpid();
    break;
  case OPEN:
    sys_open((char*)p2);
    break;
  case READ:
    sys_read(p2, (void*)p3, p4);
    break;
  case WRITE:
    sys_write(p2, (void*)p3, p4);
    break;
  }
}
