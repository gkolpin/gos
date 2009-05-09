#ifndef _CONSOLE_H
#define _CONSOLE_H

#include "types.h"

void cons_init(void);
void cons_putchar(char c);
void set_display(void*, int nbytes);

#endif
