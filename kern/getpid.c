#include "getpid.h"
#include "task.h"
#include "sched.h"

void getpid(){
  task *curTask = get_cur_task();
  set_syscall_return(curTask, curTask->id);
}
