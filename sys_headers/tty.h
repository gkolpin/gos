#ifndef _TTY_H
#define _TTY_H

#include "driver.h"

driver * tty_init();
void kb_int(void);
void kputchar(char c);

#endif
