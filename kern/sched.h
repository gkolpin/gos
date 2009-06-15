#ifndef _SCHED_H
#define _SCHED_H

#include "types.h"
#include "task.h"

/* pointer to currently executing task */
task *cur_task_p;

void sched_init();
int schedule(task*);
void unschedule(uint32 task_id);
void sched_enqueue(uint32 task_id);
void sched_dequeue(uint32 task_id);
task * get_cur_task();
task * get_task_for_id(uint32 task_id);
uint32 * get_children_for_task(uint32 task_id, int *n_tasks);
void task_finish_sleep(uintptr_t esp);
void wake_task(uint32 task_id);
void task_sleep();

/* defined in i386lib.s */
void restart_task();

#endif
