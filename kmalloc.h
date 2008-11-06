#ifndef KMALLOC_H
#define KMALLOC_H

#include "types.h"

void kmalloc_init();
/* size is in bytes */
void * kmalloc(uint32 size);
void * kfree(void*);

#endif
