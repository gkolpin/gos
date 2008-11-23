#include "sched.h"
#include "gos.h"
#include "mm.h"
#include "prot.h"
#include "kprintf.h"
#include "utility.h"
#include "task.h"
#include "vm.h"

/* head of task list, e.g. proc table */
PRIVATE task *tasks;
/* pointer to currently executing task */
task *cur_task_p = NULL;

extern void restart_task();

void sched_init(){
  tasks = NULL;
}

void schedule(task *t){
  task *curTask;

  /* when scheduling a task we'll give it the lowest non-scheduled id available */

  if (tasks == NULL){
    tasks = t;
    set_id(t, 0);
  } else {
    /* insert tasks sorted by ids */
    for (curTask = tasks; curTask != NULL; curTask = curTask->next){
      if (get_id(curTask) != 0 && curTask->prev == NULL){
	t->next = curTask;
	curTask->prev = t;
	tasks = t;
	set_id(t, 0);
	break;
      } else if (curTask->prev != NULL && 
		 get_id(curTask) - get_id(curTask->prev) > 1){
	set_id(t, get_id(curTask->prev) + 1);
	t->prev = curTask->prev;
	t->next = curTask;
	curTask->prev->next = t;
	curTask->prev = t;
	break;
      } else if (curTask->next == NULL){
	curTask->next = t;
	t->prev = curTask;
	set_id(t, get_id(curTask) + 1);
      }
    }
  }
}

void sched_int(){
  if (cur_task_p == NULL){
    cur_task_p = tasks;
  } else if (cur_task_p->next == NULL){
    cur_task_p = tasks;
  } else {
    cur_task_p = cur_task_p->next;
  }

  restart_task(cur_task_p);
}

task * get_cur_task(){
  return cur_task_p;
}
