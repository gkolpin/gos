#include "sched.h"
#include "gos.h"
#include "mm.h"
#include "prot.h"
#include "kprintf.h"
#include "utility.h"
#include "task.h"
#include "vm.h"
#include "kmalloc.h"

/* maximum number of processes that can be schedule at once */
#define MAX_PROC 128
/* task quantum is 5 ticks */
#define TICKS_QUANTUM 5

typedef struct sched_item {
  task *task;
  list_node l_node;
} sched_item;

/* proc table */
PRIVATE sched_item * proc_table[MAX_PROC];
/* runnable queue */
PRIVATE list run_queue;
/* blocked (sleeping) processes */
PRIVATE list blocked_queue;
/* sched_item of idle task - run when nothing else is available */
PRIVATE sched_item *idle_task;
/* number of scheduled procs */
PRIVATE int no_procs;

extern void idle_task_start();
PRIVATE task * create_idle_task();
PRIVATE sched_item * create_sched_item(uint32 id, uint16 ticks, 
				       task *t, sched_item *next);

/* defined in i386lib.s */
void _restart_task();
void task_finish_sleep(uintptr_t esp);
void start_sleeping_task(uintptr_t esp);

void sched_init(){
  int i;
  for (i = 0; i < MAX_PROC; i++){
    proc_table[i] = NULL;
  }

  run_queue = list_init(sched_item, l_node);
  blocked_queue = list_init(sched_item, l_node);
  no_procs = 0;

  idle_task = create_sched_item(MAX_PROC + 1, ~0, create_idle_task(), NULL);
  cur_task_p = idle_task->task;
}

PRIVATE task * create_idle_task(){
  task *retTask = create_kernel_task((uint32)idle_task_start);
  return retTask;
}

PRIVATE sched_item * create_sched_item(uint32 id, uint16 ticks, 
				       task *t, sched_item *next){
  sched_item *schedRet = (sched_item*)kmalloc(sizeof(sched_item));
  schedRet->task = t;
  schedRet->task->id = id;
  schedRet->task->ticks = ticks;
  return schedRet;
}

int schedule(task *t){
  sched_item **ppCurItem;
  int i;

  if (no_procs >= MAX_PROC)
    return -1;
  no_procs++;

  /* place into proc_table */
  for (i = 0, ppCurItem = proc_table; *ppCurItem; ppCurItem++, i++);
  *ppCurItem = create_sched_item(i, TICKS_QUANTUM, t, NULL);

  sched_enqueue((*ppCurItem)->task->id);

  return (*ppCurItem)->task->id;
}

void unschedule(uint32 task_id){
  sched_dequeue(task_id);
  kfree(proc_table[task_id]);
  proc_table[task_id] = NULL;
}

PRIVATE void set_new_running_task(sched_item *item){
  if (item == NULL){
    cur_task_p = idle_task->task;
    /*cur_task_p = NULL;
      cur_sched_item = NULL;*/
    return;
  }

  cur_task_p = item->task;

  prepare_task(cur_task_p);
}

void sched_enqueue(uint32 task_id){
  sched_item *pSchedItem = proc_table[task_id];
  /* place at end of run queue */
  list_add(run_queue, &pSchedItem->l_node);

  pSchedItem->task->ticks = TICKS_QUANTUM;

  if (cur_task_p == idle_task->task){
    set_new_running_task(proc_table[task_id]);
  }
}

void sched_dequeue(uint32 task_id){
  sched_item *schedItem = proc_table[task_id];
  list_remove(run_queue, &schedItem->l_node);
  list_node *lNode;

  if (schedItem->task == cur_task_p){
    if (NULL != (lNode = list_head(run_queue)))
      set_new_running_task(cur_obj(run_queue, list_head(run_queue), sched_item));
    else 
      set_new_running_task(NULL);
  }
}

task * get_cur_task(){
  return cur_task_p;
}

task * get_task_for_id(uint32 task_id){
  if (task_id >= MAX_PROC)
    return NULL;
  return proc_table[task_id]->task;
}

uint32 * get_children_for_task(uint32 task_id, int *n_tasks){
  uint32 *retVal = kmalloc(sizeof(uint32) * MAX_PROC);
  int i;
  int ret_i = 0;

  for (i = 0; i < MAX_PROC; i++){
    if (proc_table[i] != NULL &&
	proc_table[i]->task->parent == proc_table[task_id]->task){
      retVal[ret_i++] = i;
    }
  }

  *n_tasks = ret_i;

  return retVal;
}

void restart_task(){
  if (cur_task_p->waiting){
    cur_task_p->waiting = FALSE;
    start_sleeping_task(cur_task_p->kern_stack_sp);
  } else {
    _restart_task();
  }
}

void task_finish_sleep(uintptr_t esp){
  cur_task_p->kern_stack_sp = esp;
  sched_dequeue(cur_task_p->id);
  list_add(blocked_queue, &proc_table[cur_task_p->id]->l_node);
  restart_task();
}

void wake_task(uint32 task_id){
  sched_item *pSchedItem = proc_table[task_id];
  list_remove(blocked_queue, &pSchedItem->l_node);
  sched_enqueue(task_id);
}
