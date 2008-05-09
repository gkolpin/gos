#ifndef _TASK_H
#define _TASK_H

#include "types.h"

typedef struct task{
  /* registers */
  reg_t gs;
  reg_t fs;
  reg_t es;
  reg_t ds;
  /* registers below appear in order of pusha */
  reg_t edi;
  reg_t esi;
  reg_t ebp;
  reg_t esp_h;
  reg_t ebx;
  reg_t edx;
  reg_t ecx;
  reg_t eax;
  /* following is the layout of the handlers stack
     following an interrupt */
  reg_t eip;
  reg_t cs;
  reg_t eflags;
  reg_t esp;
  reg_t ss;

  void* stack; 			/* task's stack */
  uint32 stack_len;		/* size of stack */

} task;

task * create_task(void * code_start);

#endif
