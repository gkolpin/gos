#ifndef _SYSCALL_H
#define _SYSCALL_H

#include "types.h"

/* system calls */
enum {
  CONS_PUTCHAR,
  FORK
};

void ksyscall(uint32 p1, uint32 p2, uint32 p3, uint32 p4);

#endif
