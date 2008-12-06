#include "task.h"
#include "types.h"
#include "gos.h"
#include "prot.h"
#include "kmalloc.h"
#include "kprintf.h"
#include "vm.h"
#include "mm.h"
#include "utility.h"

/* default stack size is one page */
#define DEFAULT_STACK_SIZE 1

task * create_task(uint32 task_start_addr, uint32 executable_image_phys_addr,
		   uint32 executable_image_size){
  
  task *task_return = (task*)kmalloc(sizeof(task));
  task_return->stack_phys_pages[0] = (uint32)alloc_pages(DEFAULT_STACK_SIZE) / PAGE_SIZE;

  vm_alloc_at((void*)(task_return->stack_phys_pages[0] * PAGE_SIZE),
	      KERNEL_HEAP_START - PAGE_SIZE, PAGE_SIZE,
	      USER);
  task_return->stack_len = DEFAULT_STACK_SIZE * PAGE_SIZE;
  task_return->num_segments = 0;
  task_return->executable_file_phys_addr = executable_image_phys_addr;
  task_return->executable_file_size = executable_image_size;

  task_return->has_run = FALSE;
  kprintf("copying current page dir\n");
  task_return->pd_phys = copy_cur_page_dir();

  task_return->prev = NULL;
  task_return->next = NULL;

  /*kprintf("\n");
  kprint_int((uint32)*esp);
  kprintf("\n");*/

  task_return->gs = R3_DATA_S;
  task_return->fs = R3_DATA_S;
  task_return->es = R3_DATA_S;
  task_return->ds = R3_DATA_S;
  
  /* to be popped off by iret */
  task_return->ss = R3_DATA_S;
  task_return->esp = KERNEL_HEAP_START - 1;
  kprintf("stack loc: %x\n", (uint32)task_return->esp);
  kprintf("task loc: %x\n", (uint32)task_return);
  kprintf("task + stack size: %d\n", sizeof(task) + DEFAULT_STACK_SIZE);
  kprintf("task size: %d\n", sizeof(task));
  kprintf("stack size: %d\n", DEFAULT_STACK_SIZE);
  task_return->eflags = get_eflags() | 0x200; /* eflags (ensure interrupts enabled) */
  task_return->cs = R3_CODE_S;	/* cs */
  task_return->eip = (uint32)task_start_addr;	/* eip */

  kprintf("eip: %x\n", task_start_addr);

  /*kprint_int((uint32)task_return->stack_p);*/

  return task_return;
}

void add_task_segment(task *t, uint32 segment_phys_addr, uint32 segment_data_length,
		      uint32 segment_virt_addr, uint32 segment_size){
  t->segment_phys_addr[t->num_segments] = segment_phys_addr;
  t->segment_virt_addr[t->num_segments] = segment_virt_addr;
  t->segment_sizes[t->num_segments] = segment_size;
  t->num_segments++;

  vm_alloc_at((void*)segment_phys_addr, segment_virt_addr, segment_data_length, USER);
}

task * clone_task(task *t){
  int i;
  task *newTask;
  newTask = (task*)kmalloc(sizeof(task));

  kmemcpy(newTask, t, sizeof(task));

  newTask->has_run = FALSE;
  newTask->pd_phys = copy_cur_page_dir();
  
  for (i = 0; i < t->stack_len / PAGE_SIZE; i++){
    newTask->stack_phys_pages[i] = (uint32)alloc_pages(DEFAULT_STACK_SIZE) / PAGE_SIZE;
    kmemcpyphys(newTask->stack_phys_pages[i], t->stack_phys_pages[i], 1);
  }

  return newTask;
}

void set_syscall_return(task *t, uint32 ret_val){
  t->eax = ret_val;
}

uint32 get_id(task *t){
  return t->id;
}

void set_id(task *t, uint32 id){
  t->id = id;
}

void prepare_task(task *t){
  int stack_pages = t->stack_len / PAGE_SIZE;
  int i;

  set_pd(t->pd_phys);

  if (!t->has_run){
    t->has_run = TRUE;

    for (i = 0; i < stack_pages; i++){
      vm_alloc_at((void*)(t->stack_phys_pages[i] * PAGE_SIZE),
		  KERNEL_HEAP_START - PAGE_SIZE * (i + 1),
		  PAGE_SIZE, USER);
    }
  }

  set_pd(t->pd_phys);
}
