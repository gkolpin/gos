#include "syscall.h"
#include "gos.h"
#include "types.h"
#include "sched.h"


void kill_task(uint32 task_id){
  task *tToKill = get_task_for_id(task_id);
  if (tToKill != NULL){
    unschedule(task_id);
    task_free(tToKill);
  }
}
