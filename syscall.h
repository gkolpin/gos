#ifndef _SYSCALL_H
#define _SYSCALL_H

#include "types.h"

void fork_cur_task();
void kill_task(uint32 task_id);

/* system calls */
enum {
  CONS_PUTCHAR,
  FORK,
  KILL
};

void ksyscall(uint32 p1, uint32 p2, uint32 p3, uint32 p4);

#endif
