#ifndef _INT_H
#define _INT_H

/* restore interrupts - defined in i386lib.s */
void cmd_sti(void);
/* software interrupt */
void cmd_int(int intrpt);
/* halt processor until interrupt occurs */
void cmd_hlt();
#endif
