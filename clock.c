#include "clock.h"
#include "gos.h"
#include "types.h"

void set_ticks(uint16 ticks){
  outb(0x43, 0x36);
  outb(0x40, ticks & 0xFF);
  outb(0x40, ticks >> 8);
}

uint16 get_ticks(){
  uint32 ticks = 0;
  outb(0x43, 0x36);
  ticks = ((uint32)inb(0x40)) & 0xFF;
  ticks |= ((((uint32)inb(0x40)) & 0xFF) << 8);
  return ticks;
}
