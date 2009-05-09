#ifndef _DRIVER_H
#define _DRIVER_H

#include "types.h"

typedef struct driver {
  int (*init)(void *dev_data);
  int (*read)(void *buf, uint32 num_bytes, void *dev_data);
  int (*write)(void *buf, uint32 num_bytes, void *dev_data);
} driver;

#endif
