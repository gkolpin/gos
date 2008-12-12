#ifndef _TASK_H
#define _TASK_H

#include "types.h"

#define MAX_TASK_SEGMENTS 10
#define MAX_STACK_PAGES 10

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

  uint32 stack_len;		/* size of stack in bytes */
  uint32 stack_phys_pages[MAX_STACK_PAGES];
  uint32 num_segments;		/* number of memory segments */
  uint32 segment_phys_addr[MAX_TASK_SEGMENTS]; /* space for segment pointers */
  uint32 segment_virt_addr[MAX_TASK_SEGMENTS];
  uint32 segment_pages[MAX_TASK_SEGMENTS]; /* segment sizes in pages */

  uint32 id;			/* task's id */
  bool has_run;			/* records if the task has actually started */
  uint32 pd_phys;		/* physical address of the task's page dir */

  struct task *prev;		/* for placing tasks in lists */
  struct task *next;

} task;

task * create_task(uint32 task_start_addr);
void task_free(task*);
void add_task_segment(task*, uint32 segment_phys_addr, uint32 segment_data_length,
		      uint32 segment_virt_addr, uint32 segment_pages);
task * clone_task(task*);
void set_syscall_return(task*, uint32);
uint32 get_id(task*);
void set_id(task*, uint32);
void prepare_task(task*);

#endif
