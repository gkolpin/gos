#include "syscall.h"

#define CONS_PUTCHAR	0

void ksyscall(uint32 p1, uint32 p2, uint32 p3, uint32 p4){
  /* p1 is syscall number */
  switch(p1){
  case CONS_PUTCHAR:
    cons_putchar((uint8)p2);
    break;
  }
}
