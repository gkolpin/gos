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

  void* stack; 			/* task's stack */
  uint32 stack_len;		/* size of stack in pages */
  uint32 stack_phys_pages[MAX_STACK_PAGES];
  uint32 num_segments;		/* number of memory segments */
  uint32 segment_phys_addr[MAX_TASK_SEGMENTS]; /* space for segment pointers */
  uint32 segment_virt_addr[MAX_TASK_SEGMENTS];
  uint32 segment_sizes[MAX_TASK_SEGMENTS]; /* segment sizes in bytes */
  uint32 executable_file_phys_addr; /* physical address of executable file image */
  uint32 executable_file_size;	/* executable file size in bytes */

} task;

task * create_task(uint32 task_start_addr, uint32 executable_image_phys_addr,
		   uint32 executable_image_size);
void add_task_segment(task*, uint32 segment_phys_addr, uint32 segment_data_length,
		      uint32 segment_virt_addr, uint32 segment_size);

#endif
