#ifndef _VM_H
#define _VM_H

#include "types.h"

/* Using 4kb pages. */
#define PAGE_SIZE 4096

typedef enum user_type {
  SUPERVISOR,
  USER
} user_type;


void vm_init();

/* requested_loc is an absolute page number */
void * vm_malloc(uint32 requested_loc, uint32 size, user_type);

#endif
