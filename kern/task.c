#include "task.h"
#include "types.h"
#include "gos.h"
#include "prot.h"
#include "kmalloc.h"
#include "kprintf.h"
#include "vm.h"
#include "mm.h"
#include "utility.h"
#include "list.h"

/* default stack size is one page */
#define DEFAULT_STACK_SIZE 1

struct _mem_seg {
  uint phys_page;		/* segment physical page number */
  uintptr_t virt_addr;		/* segment virtual address */
  uint pages;			/* segment size in pages */
  list_node l_node;
};

struct _fd {
  int fd;
  vfd vfd;
  list_node l_node;
};

PRIVATE void close_all_descriptors(task*);
PRIVATE void copyDescriptors(task *from, task *to);
PRIVATE void copyMemSegmentLists(task *from, task *to);

task * create_task(uint32 task_start_addr){
  task *task_return = (task*)kmalloc(sizeof(task));
  struct _mem_seg *stack = (struct _mem_seg*)kmalloc(sizeof(struct _mem_seg));
  int i;

  task_return->kern_stack = kmalloc(PAGE_SIZE);

  for (i = 0; i < NO_SEG_TYPES; i++){
    task_return->mem_segments[i] = list_init(struct _mem_seg, l_node);
  }
  stack->phys_page = (uint)alloc_pages(DEFAULT_STACK_SIZE) / PAGE_SIZE;
  stack->virt_addr = (uint)KERNEL_HEAP_START - PAGE_SIZE;
  vm_alloc_at((void*)(stack->phys_page * PAGE_SIZE),
	      stack->virt_addr, PAGE_SIZE,
	      USER);
  stack->pages = DEFAULT_STACK_SIZE;
  list_add(task_return->mem_segments[STACK], &stack->l_node);

  task_return->data_seg_start_vaddr = 0;
  task_return->data_seg_end_vaddr = 0;

  task_return->has_run = FALSE;
  kprintf("copying current page dir\n");
  task_return->pd_phys = copy_cur_page_dir();

  task_return->parent = NULL;
  task_return->wait_for_child = FALSE;

  task_return->descriptors = list_init(struct _fd, l_node);

  task_return->waiting = FALSE;

  /*kprintf("\n");
  kprint_int((uint32)*esp);
  kprintf("\n");*/

  task_return->gs = R3_DATA_S;
  task_return->fs = R3_DATA_S;
  task_return->es = R3_DATA_S;
  task_return->ds = R3_DATA_S;
  
  /* to be popped off by iret */
  task_return->ss = R3_DATA_S;
  task_return->esp = KERNEL_HEAP_START;
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

task * create_kernel_task(uint32 task_start_addr){
  task *task_return = (task*)kmalloc(sizeof(task));

  task_return->kern_stack = kmalloc(PAGE_SIZE);

  task_return->data_seg_start_vaddr = 0;
  task_return->data_seg_end_vaddr = 0;

  task_return->has_run = FALSE;
  kprintf("copying current page dir\n");
  task_return->pd_phys = copy_cur_page_dir();

  task_return->waiting = FALSE;

  /*kprintf("\n");
  kprint_int((uint32)*esp);
  kprintf("\n");*/

  task_return->gs = R0_DATA_S;
  task_return->fs = R0_DATA_S;
  task_return->es = R0_DATA_S;
  task_return->ds = R0_DATA_S;
  
  /* to be popped off by iret */
  task_return->ss = R0_DATA_S;
  //task_return->esp = KERNEL_HEAP_START - 1;
  /*kprintf("stack loc: %x\n", (uint32)task_return->esp);
  kprintf("task loc: %x\n", (uint32)task_return);
  kprintf("task + stack size: %d\n", sizeof(task) + DEFAULT_STACK_SIZE);
  kprintf("task size: %d\n", sizeof(task));
  kprintf("stack size: %d\n", DEFAULT_STACK_SIZE);*/
  task_return->eflags = get_eflags() | 0x200; /* eflags (ensure interrupts enabled) */
  task_return->cs = R0_CODE_S;	/* cs */
  task_return->eip = (uint32)task_start_addr;	/* eip */

  kprintf("eip: %x\n", task_start_addr);

  /*kprint_int((uint32)task_return->stack_p);*/

  return task_return;
}

void task_free(task *t){
  int i;
  list_node *curNode;
  list curList;
  struct _mem_seg *curSeg;

  kfree(t->kern_stack);

  
  for (i = 0; i < NO_SEG_TYPES; i++){
    curList = t->mem_segments[i];
    while (NULL != (curNode = list_head(curList))){
      curSeg = cur_obj(curList, curNode, struct _mem_seg);
      if (!(NULL != t->parent && CODE == i)){
	free_pages((void*)(curSeg->phys_page * PAGE_SIZE), curSeg->pages);
      }
      list_remove(curList, curNode);
      kfree(curSeg);
    }
    destroy_list(curList);
  }

  pd_free(t->pd_phys);

  close_all_descriptors(t);
  destroy_list(t->descriptors);

  kfree(t);
}

void add_task_segment(task *t, uint32 segment_phys_addr, uint32 segment_data_length,
		      uint32 segment_virt_addr, uint32 segment_pages){
  uint32 seg_end;

  struct _mem_seg *seg = kmalloc(sizeof(struct _mem_seg));
  list_add(t->mem_segments[CODE], &seg->l_node);
  seg->phys_page = segment_phys_addr / PAGE_SIZE;
  seg->virt_addr = segment_virt_addr;
  seg->pages = segment_pages;

  /* increase data segment end */
  if ((seg_end = segment_virt_addr + segment_pages * PAGE_SIZE) > t->data_seg_start_vaddr){
    t->data_seg_start_vaddr = PAGE_ALIGN_UP(seg_end);
    t->data_seg_end_vaddr = t->data_seg_start_vaddr;
  }

  vm_alloc_at((void*)segment_phys_addr, segment_virt_addr, segment_data_length, USER);
}

PRIVATE void copyDescriptors(task *from, task *to){
  struct _fd *curFd;
  struct _fd *newFd;
  list_node *curNode;
  for (curNode = list_head(from->descriptors); NULL != curNode; curNode = list_next(from->descriptors, curNode)){
    curFd = cur_obj(from->descriptors, curNode, struct _fd);
    newFd = (struct _fd*)kmalloc(sizeof(struct _fd));
    kmemcpy(newFd, curFd, sizeof(struct _fd));
    newFd->vfd = vfs_copy_vfd(curFd->vfd);
    list_add(to->descriptors, &newFd->l_node);
  }
}

PRIVATE void copyMemSegmentLists(task *from, task *to){
  int i;
  list curList;
  list_node *curNode;
  struct _mem_seg *curSeg;
  struct _mem_seg *newSeg;
  for (i = 0; i < NO_SEG_TYPES; i++){
    to->mem_segments[i] = list_init(struct _mem_seg, l_node);
    curList = from->mem_segments[i];
    for (curNode = list_head(curList); NULL != curNode; curNode = list_next(curList, curNode)){
      curSeg = cur_obj(curList, curNode, struct _mem_seg);
      newSeg = kmalloc(sizeof(struct _mem_seg));
      kmemcpy(newSeg, curSeg, sizeof(struct _mem_seg));
      list_add(to->mem_segments[i], &newSeg->l_node);
    }
  }
}

PRIVATE void copyStackPages(task *from, task *to){
  list stackList_from = from->mem_segments[STACK];
  list stackList_to = to->mem_segments[STACK];
  list_node *curNode_from, *curNode_to;
  struct _mem_seg *fromSeg, *toSeg;
  for (curNode_from = list_head(stackList_from), curNode_to = list_head(stackList_to);
       NULL != curNode_from;
       curNode_from = list_next(stackList_from, curNode_from), 
	 curNode_to = list_next(stackList_to, curNode_to)){
    fromSeg = cur_obj(stackList_from, curNode_from, struct _mem_seg);
    toSeg = cur_obj(stackList_to, curNode_to, struct _mem_seg);
    toSeg->phys_page = (uint32)alloc_pages(fromSeg->pages) / PAGE_SIZE;
    kmemcpyphys(toSeg->phys_page, fromSeg->phys_page, fromSeg->pages);
  }
}

task * clone_task(task *t){
  int i;
  task *newTask;
  newTask = (task*)kmalloc(sizeof(task));

  kmemcpy(newTask, t, sizeof(task));

  newTask->kern_stack = kmalloc(PAGE_SIZE);

  newTask->has_run = FALSE;
  newTask->pd_phys = copy_cur_page_dir();
  newTask->parent = t;
  newTask->descriptors = list_init(struct _fd, l_node);

  copyDescriptors(t, newTask);

  copyMemSegmentLists(t, newTask);

  copyStackPages(t, newTask);

  return newTask;
}

void set_syscall_return(task *t, uint32 ret_val){
  t->eax = ret_val;
}

void prepare_task(task *t){
  list stack_list = t->mem_segments[STACK];
  int i;
  list_node *curNode;
  struct _mem_seg *curSeg;

  set_pd(t->pd_phys);

  if (!t->has_run){
    t->has_run = TRUE;

    for (curNode = list_head(stack_list), i = 0; NULL != curNode; 
	 curNode = list_next(stack_list, curNode), i++){
      curSeg = cur_obj(stack_list, curNode, struct _mem_seg);
      vm_alloc_at((void*)(curSeg->phys_page * PAGE_SIZE),
		  KERNEL_HEAP_START - PAGE_SIZE * (i + 1),
		  PAGE_SIZE, USER);
    }
  }

  set_pd(t->pd_phys);
}

void set_wait_for_child(task *t, uint32 child_task_id, uint32 statloc){
  t->wait_for_child = TRUE;
  t->wait_statloc = statloc;
  t->wait_child_id = child_task_id;
}

bool within_task_mem_map(task *t, uint32 virt_addr){
  int i;
  list_node *curNode;
  struct _mem_seg *curSeg;
  list curList;
  /* look in STACK and HEAP segments */
  for (i = 0; i < NO_SEG_TYPES; i++){
    if (CODE == i)
      continue;
    curList = t->mem_segments[i];
    for (curNode = list_head(curList); NULL != curNode;
	 curNode = list_next(curList, curNode)){
      curSeg = cur_obj(curList, curNode, struct _mem_seg);
      if (virt_addr >= curSeg->virt_addr &&
	  virt_addr < (curSeg->virt_addr + PAGE_SIZE * curSeg->pages)){
	return TRUE;
      }
    }
  }

  return FALSE;
}

void task_set_mem(task *t, void *dest, void *src, uint32 len){
  void *phys_addr = virt2phys(t->pd_phys, dest);
  kmemcpy2phys(phys_addr, src, len);
}

uint32 get_data_heap_end(task *t){
  return t->data_seg_end_vaddr;
}

PRIVATE uint32 stack_len(task *t){
  list l = t->mem_segments[STACK];
  list_node *n;
  struct _mem_seg *seg;
  uint32 len = 0;
  for (n = list_head(l); NULL != n; n = list_next(l, n)){
    seg = cur_obj(l, n, struct _mem_seg);
    len += seg->pages * PAGE_SIZE;
  }
  return len;
}

bool move_data_heap_end(task *t, int amnt){
  uint32 old_data_end = t->data_seg_end_vaddr;
  uint32 new_data_end = old_data_end + amnt;
  uint32 numBytes;
  void *new_mem;
  struct _mem_seg *newSeg;

  kprintf("old_data_end: %d, new_data-end: %d\n", old_data_end, new_data_end);

  if (new_data_end < t->data_seg_start_vaddr ||
      new_data_end > KERNEL_HEAP_START - stack_len(t)){
    return FALSE;
  }
  
  t->data_seg_end_vaddr = new_data_end;
  /* for now we'll just handle the case where we're growing the data segment */
  if (new_data_end > old_data_end &&
      (numBytes = PAGE_ALIGN_UP(new_data_end) - PAGE_ALIGN_UP(old_data_end)) > 0){
    kprintf("allocating new pages!\n");
    /* need to allocate more pages */
    newSeg = kmalloc(sizeof(struct _mem_seg));
    new_mem = alloc_pages(PAGES_FOR_BYTES(numBytes));
    newSeg->phys_page = (uintptr_t)new_mem / PAGE_SIZE;
    newSeg->pages = PAGES_FOR_BYTES(numBytes);
    vm_alloc_at(new_mem, PAGE_ALIGN_UP(old_data_end), numBytes, USER);
    newSeg->virt_addr = PAGE_ALIGN_UP(old_data_end);
    list_add(t->mem_segments[HEAP], &newSeg->l_node);
  }

  return TRUE;
}

int task_add_vfd(task *t, vfd vfd){
  struct _fd *fd = (struct _fd*)kmalloc(sizeof(struct _fd));
  fd->vfd = vfd;
  list_node *cur_node;
  struct _fd *curfd;
  int i;
  for (cur_node = list_head(t->descriptors), i = 1; cur_node != NULL; cur_node = list_next(t->descriptors, cur_node), i++){
    curfd = cur_obj(t->descriptors, cur_node, struct _fd);
    if (curfd->fd > i){
      fd->fd = i;
      list_insert_before(cur_node, &(fd->l_node));
      return i;
    }
  }
  
  fd->fd = i;
  list_add(t->descriptors, &(fd->l_node));
  return i;
}

PRIVATE void close_all_descriptors(task *t){
  list_node *n;
  struct _fd *fd;
  list_node *tmp;
  for (n = list_head(t->descriptors); n != NULL; ){
    fd = cur_obj(t->descriptors, n, struct _fd);
    vfs_close(fd->vfd);
    tmp = list_next(t->descriptors, n);
    kfree(fd);
    n = tmp;
  } 
}

vfd task_get_vfd(task *t, int d){
  list_node *n;
  struct _fd *fd;
  for (n = list_head(t->descriptors); n != NULL && d > 0; d--);
  
  if (n != NULL){
    fd = cur_obj(t->descriptors, n, struct _fd);
    return fd->vfd;
  }
  
  return NULL;
}
