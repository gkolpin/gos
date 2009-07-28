#include "gos.h"
#include "types.h"
#include "syscall.h"
#include "sched.h"
#include "kmalloc.h"

#define ERROR -1

void waitpid(uint32 pid, uint32 statloc, uint32 opts){
  /* ensure pid is child process */
  int no_tasks;
  uint32 *children;
  bool found = FALSE;
  int i;

  if (pid != -1){
    children = get_children_for_task(cur_task_p->id, &no_tasks);
    for (i = 0; i < no_tasks; i++){
      if (children[i] == pid){
	found = TRUE;
	break;
      }
    }
    
    if (!found){
      /* return ERROR to caller */
      set_syscall_return(cur_task_p, ERROR);
    }

    kfree(children);
  }
  
  /* ensure statloc is address within process's mem map */
  if (!within_task_mem_map(cur_task_p, statloc)){
    set_syscall_return(cur_task_p, ERROR);
  }

  set_wait_for_child(cur_task_p, pid, statloc);

  sched_dequeue(cur_task_p->id);
}
