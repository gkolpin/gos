#ifndef _SCHED_H
#define _SCHED_H

#include "types.h"
#include "task.h"

typedef struct sched_item {
  uint32 id;			/* task id and index into proc_table */
  uint16 ticks;			/* ticks remaining */
  task *task;
  
  struct sched_item *next;
} sched_item;

/* pointer to currently executing task */
task *cur_task_p;
sched_item *cur_sched_item;

void sched_init();
int schedule(task*);
void unschedule(uint32 task_id);
void sched_enqueue(uint32 task_id);
void sched_dequeue(uint32 task_id);
task * get_cur_task();
task * get_task_for_id(uint32 task_id);

/* defined in i386lib.s */
void restart_task();

#endif
