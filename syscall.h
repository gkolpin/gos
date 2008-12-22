#ifndef _SYSCALL_H
#define _SYSCALL_H

#include "types.h"

void fork_cur_task();
void kill_task(uint32 task_id);
void get_uptime();
void exit_cur_task(int status);

/* system calls */
enum {
  CONS_PUTCHAR,
  FORK,
  EXIT,
  KILL,
  UPTIME
};

void ksyscall(uint32 p1, uint32 p2, uint32 p3, uint32 p4);

#endif
