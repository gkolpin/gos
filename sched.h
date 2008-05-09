#ifndef _SCHED_H
#define _SCHED_H

#include "types.h"
#include "task.h"

void sched_init();
void schedule(task*);
void sched_int();

#endif
