#ifndef _TASK_H
#define _TASK_H

#include "types.h"
#include "vfs.h"
#include "list.h"
#include "wait.h"

#define MAX_TASK_SEGMENTS 10
#define MAX_STACK_PAGES 10

enum SEG_TYPES {
  CODE = 0,
  STACK,
  HEAP,
  NO_SEG_TYPES
};

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

  void *kern_stack;		/* per-task kernel stack space */
  uintptr_t kern_stack_sp;	/* kernel stack pointer */

  list mem_segments[NO_SEG_TYPES];	/* code, stack, and heap segments */

  uint32 data_seg_start_vaddr;	/* data segment start virtual address - page aligned */
  uint32 data_seg_end_vaddr;	/* data segment end virtual address - not page aligned*/

  bool has_run;			/* records if the task has actually started */
  uint32 pd_phys;		/* physical address of the task's page dir */

  uint32 id;			/* task id */
  uint16 ticks;			/* ticks remaining */

  int exit_status;
  struct task *parent;
  bool wait_for_child;
  uint32 wait_child_id;
  uint32 wait_statloc;
  int child_exit_status;

  list descriptors;
  int sid;
  int pgid;

  waitlink w_link;
  bool waiting;
} task;

task * create_task(uint32 task_start_addr);
task * create_kernel_task(uint32 task_start_addr);
void task_free(task*);
void add_task_segment(task*, uint32 segment_phys_addr, uint32 segment_data_length,
		      uint32 segment_virt_addr, uint32 segment_pages);
task * clone_task(task*);
void set_syscall_return(task*, uint32);
void prepare_task(task*);
void set_wait_for_child(task*, uint32 child_task_id, uint32 statloc);
bool within_task_mem_map(task*, uint32 virt_addr);
void task_set_mem(task*, void* dest, void* src, uint32 len);
uint32 get_data_heap_end(task*);
bool move_data_heap_end(task*, int amnt);
int task_add_vfd(task*, vfd);
vfd task_get_vfd(task*, int fd);

#endif
