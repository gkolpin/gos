#ifndef _MM_H
#define _MM_H

/* allocates the specified amount of memory */
void * malloc(uint32);
/* allocate the memory manager */
/* argument is number of 1024 byte blocks of extended memory */
void mm_init(uint32);

#endif
