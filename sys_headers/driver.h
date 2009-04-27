#ifndef _DRIVER_H
#define _DRIVER_H

#include "types.h"

typedef struct driver {
  int (*init)();
  int (*read)(void *buf, uint32 num_bytes);
  int (*write)(void *buf, uint32 num_bytes);
} driver;

#endif
