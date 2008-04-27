#ifndef _SCHED_H
#define _SCHED_H

#include "types.h"

typedef struct task{
  void* stack;
  uint32 stack_len;
  uint32* stack_p;
} task;

void sched_init();
task * create_task(void * code_start);
void schedule(task*);
void sched_int();

#endif
