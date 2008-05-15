#ifndef _AT_DISK_DRIVER_H
#define _AT_DISK_DRIVER_H

#include "types.h"

void disk_read_sector(uint32 sector, uint8* buf);

#endif
