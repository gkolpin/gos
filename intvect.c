#include "gos.h"
#include "intvect.h"
#include "int.h"
#include "prot.h"

/* define external reference to IDT table */
extern void IDT;
extern void LIDTR;
extern KEYBOARD_INT(void);
extern TIMER_INT(void);
extern SYSCALL_INT(void);
extern GEN_INT(void);
extern ERR_INT(void);

/* defined in i386lib.s */
extern void cmd_lidt(void*);

/* this is the value for the flags in the first 16 bits  */
/* of the second double-word of the interrupt gate descriptor */
#define INT_VECT_FLAGS	0x4700
/* index into gdt for code segment */
#define CODE_SELECTOR	0x8
/* keyboard vector */
#define KB_VECT        	33
/* timer vector */
#define TIMER_VECT	32
/* sys call int */
#define SYSCALL_VECT	50

void intvect_init(void){
  int i;
  uint32 *lIDT = &IDT;

  /* now load the idt */
  cmd_lidt(&LIDTR);

  for (i = 0; i < (2 * 256); i+=2){
    if (i == (KB_VECT * 2)){
      lIDT[i] 	|= ((CODE_SELECTOR) << 16);
      lIDT[i] 	|= ((uint32)KEYBOARD_INT & 0xFFFF);
      lIDT[i+1]	|= ((uint32)KEYBOARD_INT & 0xFFFF0000);
      lIDT[i+1]	|= 0x8E00;
    }
    else if (i == (TIMER_VECT * 2)){
      lIDT[i] 	|= ((CODE_SELECTOR) << 16);
      lIDT[i] 	|= ((uint32)TIMER_INT & 0xFFFF);
      lIDT[i+1]	|= ((uint32)TIMER_INT & 0xFFFF0000);
      lIDT[i+1]	|= 0x8E00;
    }
    else if (i == (SYSCALL_VECT * 2)){
      lIDT[i] 	|= ((R0_CODE_S) << 16);
      lIDT[i] 	|= ((uint32)SYSCALL_INT & 0xFFFF);
      lIDT[i+1]	|= ((uint32)SYSCALL_INT & 0xFFFF0000);
      lIDT[i+1]	|= 0xEE00;	/* descriptor privelege level is 3 */
      //lIDT[i+1]	|= 0x8E00;
    }
    else if (i == (8 * 2) || ((i >= 10 * 2) && (i < 15 * 2))){
      /* it's an error */
      lIDT[i]	|= ((CODE_SELECTOR) << 16);
      lIDT[i]	|= ((uint32)ERR_INT & 0xFFFF);
      lIDT[i+1]	|= ((uint32)ERR_INT & 0xFFFF0000);
      lIDT[i+1]	|= 0x8E00;
    }
    else {
      lIDT[i] 	|= ((CODE_SELECTOR) << 16);
      lIDT[i] 	|= ((uint32)GEN_INT & 0xFFFF);
      lIDT[i+1]	|= ((uint32)GEN_INT & 0xFFFF0000);
      lIDT[i+1]	|= 0x8E00;
    }
  }
  
  /* restore interrupts */
  //cmd_sti();
}

