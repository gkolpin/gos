#ifndef _SCHED_H
#define _SCHED_H

#include "types.h"

typedef struct task{
  /* registers */
  uint32 gs;
  uint32 fs;
  uint32 es;
  uint32 ds;
  /* registers below appear in order of pusha */
  uint32 edi;
  uint32 esi;
  uint32 ebp;
  uint32 esp_h;
  uint32 ebx;
  uint32 edx;
  uint32 ecx;
  uint32 eax;
  /* following is the layout of the handlers stack
     following an interrupt */
  uint32 eip;
  uint32 cs;
  uint32 eflags;
  uint32 esp;
  uint32 ss;

  void* stack; 			/* task's stack */
  uint32 stack_len;		/* size of stack */

} task;

void sched_init();
task * create_task(void * code_start);
void schedule(task*);
void sched_int();

#endif
