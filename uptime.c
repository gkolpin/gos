#include "gos.h"
#include "types.h"
#include "syscall.h"
#include "clock.h"
#include "task.h"
#include "sched.h"

/* return number of seconds the machine has been on */
void get_uptime(){
  set_syscall_return(get_cur_task(), get_seconds_since_boot());
}
