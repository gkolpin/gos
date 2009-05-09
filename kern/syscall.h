#ifndef _SYSCALL_H
#define _SYSCALL_H

#include "types.h"

void fork_cur_task();
void kill_task(uint32 task_id);
void get_uptime();
void exit_cur_task(int status);
void waitpid(uint32 pid, uint32 statloc, uint32 opts);
void brk(void *addr);
void sbrk(int incr);
void sys_open(char *path);
void sys_read(int d, void *buf, size_t nbytes);
void sys_write(int d, void *buf, size_t nbytes);

#include "syscalls.h"

void ksyscall(uint32 p1, uint32 p2, uint32 p3, uint32 p4);

#endif
