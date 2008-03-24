#ifndef _PORTIO_H
#define _PORTIO_H

#include "types.h"

void outb(uint16 port, uint8 data);
uint8 inb(uint16 port);

#endif
