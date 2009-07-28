#include "clock.h"
#include "gos.h"
#include "types.h"
#include "portio.h"
#include "sched.h"

/* clock 'ticks' 60 times a second */
#define CLOCK_FREQ 60
/* generates a 60 Hz pulse */
#define CLOCK_COUNTER 19887
/* port to set clock mode */
#define CLOCK_MODE_PORT 0x43
/* port to set clock counter */
#define TIMER_COUNTER_PORT 0x40
/* set 16-bit square wave pulse */
#define CLOCK_MODE 0x36

PRIVATE uint32 ticks_since_boot;

void clock_init(){
  outb(CLOCK_MODE_PORT, CLOCK_MODE);
  outb(TIMER_COUNTER_PORT, CLOCK_COUNTER & 0xFF);
  outb(TIMER_COUNTER_PORT, CLOCK_COUNTER >> 8);

  ticks_since_boot = 0;
}

void clock_tick(){
  /* this gets called at each timer interrupt - 60 times a second */
  int cur_id;

  ticks_since_boot++;

  cur_task_p->ticks--;

  if (cur_task_p->ticks <= 0){
    /* schedule a new task */
    sched_dequeue(cur_id = cur_task_p->id);
    sched_enqueue(cur_id);
  }
}

uint32 get_seconds_since_boot(){
  return ticks_since_boot / CLOCK_FREQ;
}
