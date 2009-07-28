#include "pf.h"
#include "task.h"
#include "sched.h"
#include "syscall.h"

void handle_page_fault(uint32 error_code){
  /*kprintf("P: %d, WR: %d, US: %d\n", error_code & 1, (error_code >> 1) & 1, 
	  (error_code >> 2) & 1);
	  while (1);*/

  kill_task((get_cur_task())->id);
}
