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
  task *t = get_cur_task();
  vfd vfd = task_get_vfd(t, d);
  int ret;
  if (!within_task_mem_map(t, (uintptr_t)buf) ||
      !(within_task_mem_map(t, (uintptr_t)buf + nbytes))){
    set_syscall_return(t, -1);
    return;
  }
  ret = vfs_read(vfd, buf, nbytes);
  if (0 == ret){
    /* set task to block */
  }
  set_syscall_return(t, ret);
}

void sys_write(int d, void *buf, size_t nbytes){
  task *t = get_cur_task();
  vfd vfd = task_get_vfd(t, d);
  int ret;
  if (!within_task_mem_map(t, (uintptr_t)buf) ||
      !(within_task_mem_map(t, (uintptr_t)buf + nbytes))){
    set_syscall_return(t, -1);
    return;
  }
  ret = vfs_write(vfd, buf, nbytes);
  set_syscall_return(t, ret);
}

