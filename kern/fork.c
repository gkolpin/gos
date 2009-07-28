#include "syscall.h"
#include "sched.h"
#include "kprintf.h"

void fork_cur_task(){
  task *curTask = get_cur_task();
  task *newTask;
  int newTaskId;

  kprintf("forking..\n");

  newTask = clone_task(curTask);

  kprintf("created new task struct\n");

  /* schedule will assign an id to the task */
  newTaskId = schedule(newTask);

  kprintf("scheduled\n");
  kprintf("new task addr: %x\n", (uint32)newTask);
  kprintf("new task size: %d\n", sizeof(task));
  
  /* 'return' child proc number to parent and 0 in child */
  set_syscall_return(newTask, 0);
  set_syscall_return(curTask, newTaskId);
}
