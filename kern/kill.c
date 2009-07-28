#include "syscall.h"
#include "gos.h"
#include "types.h"
#include "sched.h"
#include "task.h"
#include "kmalloc.h"

PRIVATE void kill_child_tasks(uint32 task_id){
  int no_children;
  uint32 *children = get_children_for_task(task_id, &no_children);
  int i;
  for (i = 0; i < no_children; i++){
    kill_task(children[i]);
  }
  kfree(children);
}

PRIVATE void do_waitpid(task *tWaiting, task *child_task){
  set_syscall_return(tWaiting, child_task->id);
  tWaiting->child_exit_status = child_task->exit_status;
  task_set_mem(tWaiting, (void*)tWaiting->wait_statloc, 
	       (void*)&child_task->exit_status, sizeof(int));
  tWaiting->wait_for_child = FALSE;
  sched_enqueue(tWaiting->id);
}

void kill_task(uint32 task_id){
  task *tToKill;

  kill_child_tasks(task_id);

  tToKill = get_task_for_id(task_id);

  if (tToKill && tToKill->parent && tToKill->parent->wait_for_child){
    if (tToKill->parent->wait_child_id == -1 ||
	tToKill->parent->wait_child_id == task_id){
      do_waitpid(tToKill->parent, tToKill);
    }
  }

  if (tToKill != NULL){
    unschedule(task_id);
    task_free(tToKill);
  }
}
