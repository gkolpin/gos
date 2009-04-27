#ifndef _PORTIO_H
#define _PORTIO_H

#include "types.h"

void outb(uint16 port, uint8 data);
uint8 inb(uint16 port);
void outs(uint16 port, word_t* data, uint32 size);
void ins(uint16 port, word_t* buf, uint32 size);

#endif
