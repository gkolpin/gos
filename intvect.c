#include "gos.h"
#include "intvect.h"
#include "int.h"
#include "prot.h"
#include "vm.h"

/* define external reference to IDT table */
extern void IDT;
extern void LIDTR;
extern void KEYBOARD_INT(void);
extern void TIMER_INT(void);
extern void SYSCALL_INT(void);
extern void GEN_INT(void);
extern void ERR_INT(void);
extern void DOUBLE_FAULT_INT(void);
extern void INVALID_TSS_INT(void);
extern void SEGMENT_NP_FAULT_INT(void);
extern void STACK_FAULT_INT(void);
extern void GENERAL_PROT_FAULT_INT(void);
extern void PAGE_FAULT_INT(void);

/* defined in i386lib.s */
extern void cmd_lidt(void*);

/* this is the value for the flags in the first 16 bits  */
/* of the second double-word of the interrupt gate descriptor */
#define INT_VECT_FLAGS	0x4700
/* index into gdt for code segment */
#define CODE_SELECTOR	R0_CODE_S
/* keyboard vector */
#define KB_VECT        	33
/* timer vector */
#define TIMER_VECT	32
/* sys call int */
#define SYSCALL_VECT	50
#define DOUBLE_FAULT	8
#define INVALID_TSS	10
#define SEGMENT_NP_FAULT	11
#define STACK_FAULT	12
#define GENERAL_PROT_FAULT	13
#define PAGE_FAULT	14
/* number of entries in the IDT */
#define IDT_NUM_ENTRIES	256

#define SET_INT_ROUTINE(vect_loc, routine, privilege) (*vect_loc = \
					create_iv_entry(routine, privilege))

typedef qword_t iv_entry;

PRIVATE iv_entry create_iv_entry(void (*int_proc)(void), user_type privilege_level){
  iv_entry retVal;
  retVal.dwords[0] = retVal.dwords[1] = 0;
  retVal.dwords[0] 	|= (CODE_SELECTOR << 16);
  retVal.dwords[0] 	|= ((uint32)int_proc & 0xFFFF);
  retVal.dwords[1]	|= ((uint32)int_proc & 0xFFFF0000);
  retVal.dwords[1]	|= (privilege_level == SUPERVISOR ? 0x8E00 : 0xEE00);
  return retVal;
}

void intvect_init(void){
  int i;
  iv_entry *lIDT = &IDT;

  /* now load the idt */
  cmd_lidt(&LIDTR);

  for (i = 0; i < IDT_NUM_ENTRIES; i++, lIDT++){
    switch (i) {
    case KB_VECT: SET_INT_ROUTINE(lIDT, KEYBOARD_INT, SUPERVISOR); break;
    case TIMER_VECT: SET_INT_ROUTINE(lIDT, TIMER_INT, SUPERVISOR); break;
    case SYSCALL_VECT: SET_INT_ROUTINE(lIDT, SYSCALL_INT, USER); break;
    case DOUBLE_FAULT: SET_INT_ROUTINE(lIDT, DOUBLE_FAULT_INT, SUPERVISOR); break;
    case INVALID_TSS: SET_INT_ROUTINE(lIDT, INVALID_TSS_INT, SUPERVISOR); break;
    case SEGMENT_NP_FAULT: SET_INT_ROUTINE(lIDT, SEGMENT_NP_FAULT_INT, SUPERVISOR); break;
    case STACK_FAULT: SET_INT_ROUTINE(lIDT, STACK_FAULT_INT, SUPERVISOR); break;
    case GENERAL_PROT_FAULT: SET_INT_ROUTINE(lIDT, GENERAL_PROT_FAULT_INT, SUPERVISOR); break;
    case PAGE_FAULT: SET_INT_ROUTINE(lIDT, PAGE_FAULT_INT, SUPERVISOR); break;
    default: SET_INT_ROUTINE(lIDT, GEN_INT, SUPERVISOR); break;
    }
  }
  
  /* restore interrupts */
  //cmd_sti();
}

