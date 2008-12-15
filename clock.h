#ifndef _CLOCK_H
#define _CLOCK_H

#include "types.h"

void clock_init();
void clock_tick();
uint32 get_seconds_since_boot();

#endif
