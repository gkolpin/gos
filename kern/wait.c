#include "gos.h"
#include "wait.h"
#include "list.h"
#include "sched.h"

void waitpoint_wait(waitpoint wp){
  /* add waitlink to waitpoint  */
  list_add((list)wp, (list_node*)&cur_task_p->w_link);

  cur_task_p->waiting = TRUE;

  /* put task to sleep */
  task_sleep();
}

void waitpoint_wake_all(waitpoint wp){
  waitlink *curWaitLink;
  task *curTask;
  while ((curWaitLink = list_head((list)wp)) != NULL){
    curTask = cur_obj(wp, curWaitLink, task);
    list_remove((list)wp, (list_node*)curWaitLink);
    wake_task(curTask->id);
  }
}
