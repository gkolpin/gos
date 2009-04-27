#include "sched.h"
#include "task.h"

void brk(void *addr){
  task *curTask = get_cur_task();
  if (move_data_heap_end(curTask, get_data_heap_end(curTask) - (uint32)addr))
    set_syscall_return(curTask, 0);
  else 
    set_syscall_return(curTask, -1);
}

void sbrk(int incr){
  task *curTask = get_cur_task();
  if (move_data_heap_end(curTask, incr))
    set_syscall_return(curTask, get_data_heap_end(curTask));
  else
    set_syscall_return(curTask, -1);
}
