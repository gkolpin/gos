#include "sched.h"
#include "gos.h"
#include "mm.h"
#include "prot.h"
#include "kprintf.h"
#include "utility.h"
#include "task.h"
#include "vm.h"

#define NUM_TASKS 2

/* for now provide space to allow 2 tasks to run */
PRIVATE task tasks[NUM_TASKS];
/* index into tasks array - currently executing task */
PRIVATE int cur_task;
/* number of tasks in the system */
PRIVATE int num_tasks;
/* pointer to currently executing task */
task *cur_task_p = NULL;

extern void restart_task();

void sched_init(){
  cur_task = 0;
  num_tasks = 0;
}

void schedule(task *t){
  if (num_tasks == NUM_TASKS){
    return;
  }
  
  kmemcpy(&tasks[num_tasks], t, sizeof(task));
  /*tasks[num_tasks] = *t;*/
  cur_task = num_tasks;
  num_tasks++;
  cur_task_p = &tasks[cur_task];
  //restart_task(cur_task_p);
}

void sched_int(){
  cur_task = (cur_task + 1) % num_tasks;
  cur_task_p = &tasks[cur_task];

  /* map task into memory */
  /* for now tasks are mapped at memory address 0x100000 */
  //vm_malloc(0x100, 0x800);	/* size is 2KB */

  restart_task(cur_task_p);
}
