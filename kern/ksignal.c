#include "gos.h"
#include "types.h"
#include "ksignal.h"
#include "portio.h"
#include "int.h"

/* ticks per second */
#define TICKS_SEC 	1193200
/* maximum amount of ticks that can be set (when timer set to 0) */
#define MAX_TICKS	0xFFFF

/* keeps track of the amount of time left (in ticks) */
PRIVATE uint32 	ms_ticks_left;
PRIVATE bool 	timer_complete;
PRIVATE bool	timer_started;

PRIVATE void set_timer(void);
PRIVATE void k_halt_wait(void);

/* sleeps a certain amount of milliseconds */
void sleep(uint32 ms){
  uint32 ticks_to_sleep = (ms * TICKS_SEC) / 1000;
  uint32 tts = (uint32)ticks_to_sleep;
  ms_ticks_left = tts;
  timer_complete = FALSE;
  timer_started = TRUE;
  set_timer();
  k_halt_wait();
}

PRIVATE void k_halt_wait(){
  while (timer_started && !timer_complete){
    cmd_hlt();
  }

  timer_started = FALSE;
  timer_complete = FALSE;
}

PRIVATE void set_timer(){
  uint32 ticks;
  outb(0x43, 0x36);		/* set timer for 16 bit */
  if (ms_ticks_left > MAX_TICKS){
    /* load maximum value (0) */
    outb(0x40, 0); 		/* load LSB of timer */
    outb(0x40, 0);		/* load MSB of timer */
    ms_ticks_left -= MAX_TICKS;
  }
  else {
    ticks = ms_ticks_left;
    outb(0x40, ticks & 0xFF);	/* load LSB of timer */
    outb(0x40, ticks >> 8);	/* load MSB of timer */
    ms_ticks_left -= ticks;
  }
}

void timer_int(void){

  if (timer_started && ms_ticks_left == 0){
    timer_complete = TRUE;
  }

  set_timer();

  /* eoi to controller 2 */
  outb(0xA0, 0x20);
  /* eoi to controller 1 */
  outb(0x20, 0x20);
}

