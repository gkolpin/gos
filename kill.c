#include "syscall.h"
#include "gos.h"
#include "types.h"
#include "sched.h"


PRIVATE kill_child_tasks(uint32 task_id){
  int no_children;
  uint32 *children = get_children_for_task(task_id, &no_children);
  int i;
  for (i = 0; i < no_children; i++){
    kill_task(children[i]);
  }
  kfree(children);
}

void kill_task(uint32 task_id){
  task *tToKill;

  kill_child_tasks(task_id);

  tToKill = get_task_for_id(task_id);

  if (tToKill != NULL){
    unschedule(task_id);
    task_free(tToKill);
  }
}
