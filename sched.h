#ifndef _SCHED_H
#define _SCHED_H

#include "types.h"
#include "task.h"

void sched_init();
void schedule(task*);
void unschedule(uint32 task_id);
void sched_int();
task * get_cur_task();
task * get_task_for_id(uint32 task_id);

#endif
