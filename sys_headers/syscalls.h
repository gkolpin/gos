#ifndef _SYSCALLS_H
#define _SYSCALLS_H

/* system calls */
enum {
  CONS_PUTCHAR,
  FORK,
  EXIT,
  WAITPID,
  KILL,
  UPTIME,
  BRK,
  SBRK,
  GETPID,
  OPEN,
  READ,
  WRITE
};

#endif
