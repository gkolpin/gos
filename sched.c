#include "sched.h"
#include "gos.h"
#include "mm.h"
#include "kprintf.h"

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
  uint32 **esp;

  task *task_return = malloc(sizeof(task) + DEFAULT_STACK_SIZE);
  if (task_return == NULL){
    kprintf("Error allocating memory\n");
  }
  task_return->stack = task_return + sizeof(task);
  task_return->stack_len = DEFAULT_STACK_SIZE;
  task_return->stack_p = task_return->stack + DEFAULT_STACK_SIZE - 1;

  esp = &task_return->stack_p;

  kprintf("\n");
  kprint_int((uint32)*esp);
  kprintf("\n");

  **(esp) = get_eflags() | 0x200; /* eflags (ensure interrupts enabled) */
  esp[0]--;
  **(esp) = 0x8;	/* cs */
  esp[0]--;
  **(esp) = (uint32)code_start;	/* eip */

  esp[0] -= 8; 		/* simulate pusha */

  kprint_int((uint32)task_return->stack_p);

  return task_return;
}

void schedule(task *t){
  if (num_tasks == NUM_TASKS){
    return;
  }
  
  tasks[num_tasks] = *t;
  cur_task = num_tasks;
  num_tasks++;
  cur_task_p = &tasks[cur_task];
  //restart_task(cur_task_p);
}

void sched_int(){
  cur_task = (cur_task + 1) % num_tasks;
  cur_task_p = &tasks[cur_task];

  /* eoi to controller 2 */
  outb(0xA0, 0x20);
  /* eoi to controller 1 */
  outb(0x20, 0x20);

  restart_task(cur_task_p);
}
