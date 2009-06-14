#ifndef _SYSLIB_H
#define _SYSLIB_H

#include "types.h"

void cons_putchar(char c);
int fork();
int kill();
uint32 uptime();
void exit(int status);
void waitpid(uint32 pid, uint32 *status, uint32 opts);
void wait(uint32 *status);
int brk(const void *addr);
void * sbrk(int incr);
int getpid();
int open(char *path);
int read(int d, void*, size_t);
int write(int d, void*, size_t);
int ioctl(int d, int request, void*);

#endif
