#include "syscall.h"
#include "gos.h"
#include "types.h"
#include "task.h"
#include "sched.h"

void exit_cur_task(int status){
  task *curTask = get_cur_task();
  
  /* check if this task's parent is waiting for child exit */
  /*if (is_wait_for_child(proc_table[task_id]->task->parent)){
    
  }*/

  kill_task(curTask->id);
}
