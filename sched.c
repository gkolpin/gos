#include "sched.h"
#include "gos.h"
#include "mm.h"
#include "prot.h"
#include "kprintf.h"
#include "utility.h"

#define NUM_TASKS 2
#define DEFAULT_STACK_SIZE 0x1000

/* for now provide space to allow 2 tasks to run */
PRIVATE task tasks[NUM_TASKS];
/* index into tasks array - currently executing task */
PRIVATE int cur_task;
/* number of tasks in the system */
PRIVATE int num_tasks;
/* pointer to currently executing task */
task *cur_task_p = NULL;

void restart_task(task*);
uint32 get_eflags();

void sched_init(){
  cur_task = 0;
  num_tasks = 0;
}

task * create_task(void *code_start){
  task *task_return = malloc(sizeof(task) + DEFAULT_STACK_SIZE);
  if (task_return == NULL){
    kprintf("Error allocating memory\n");
  }
  task_return->stack = task_return + sizeof(task);
  task_return->stack_len = DEFAULT_STACK_SIZE;

  /*kprintf("\n");
  kprint_int((uint32)*esp);
  kprintf("\n");*/

  task_return->gs = R3_DATA_S;
  task_return->fs = R3_DATA_S;
  task_return->es = R3_DATA_S;
  task_return->ds = R3_DATA_S;
  
  /* to be popped off by iret */
  task_return->ss = R3_DATA_S;
  task_return->esp = (uint32)(task_return->stack + task_return->stack_len - 1);
  task_return->eflags = get_eflags() | 0x200; /* eflags (ensure interrupts enabled) */
  task_return->cs = R3_CODE_S;	/* cs */
  task_return->eip = (uint32)code_start;	/* eip */

  /*kprint_int((uint32)task_return->stack_p);*/

  return task_return;
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

  restart_task(cur_task_p);
}
