#include "task.h"
#include "types.h"
#include "gos.h"
#include "prot.h"
#include "mm.h"
#include "vm.h"

#define DEFAULT_STACK_SIZE 0x1000

task * create_task(void *code_start){
  //task *task_return = (task*)malloc(sizeof(task) + DEFAULT_STACK_SIZE);
  task *task_return = (task*)vm_malloc(0x300000, sizeof(task) + DEFAULT_STACK_SIZE, USER);
  if (task_return == NULL){
    kprintf("Error allocating memory\n");
  }
  task_return->stack = (void*)((uint32)task_return + sizeof(task));
  task_return->stack_len = DEFAULT_STACK_SIZE;

  /*kprintf("\n");
  kprint_int((uint32)*esp);
  kprintf("\n");*/

  task_return->gs = R3_DATA_S;
  task_return->fs = R3_DATA_S;
  task_return->es = R3_DATA_S;
  task_return->ds = R3_DATA_S;
  
  /* to be popped off by iret */
  task_return->ss = R3_DATA_S;
  task_return->esp = (uint32)(task_return->stack + task_return->stack_len - 1);
  kprintf("stack loc: \n");
  kprint_int((uint32)task_return->esp);
  kprintf("task loc: ");
  kprint_int((uint32)task_return);
  kprintf("task + stack size: ");
  kprint_int(sizeof(task) + DEFAULT_STACK_SIZE);
  kprintf("task size: ");
  kprint_int((uint32)sizeof(task));
  kprintf("stack size: ");
  kprint_int(DEFAULT_STACK_SIZE);
  kprintf("top stack - bottom stack: ");
  kprint_int(task_return->esp - (uint32)task_return->stack);
  task_return->eflags = get_eflags() | 0x200; /* eflags (ensure interrupts enabled) */
  task_return->cs = R3_CODE_S;	/* cs */
  task_return->eip = (uint32)code_start;	/* eip */

  /*kprint_int((uint32)task_return->stack_p);*/

  return task_return;
}
