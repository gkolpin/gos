#include "types.h"
#include "vfs.h"
#include "task.h"
#include "sched.h"

void sys_open(const char *path){
  vfd vfd = vfs_open(path);
  task *curTask = get_cur_task();
  int fd = task_add_vfd(curTask, vfd);
  set_syscall_return(curTask, fd);
}

void sys_read(int d, void *buf, size_t nbytes){

}

void sys_write(int d, void *buf, size_t nbytes){

}

