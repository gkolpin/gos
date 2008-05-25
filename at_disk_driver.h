#ifndef _AT_DISK_DRIVER_H
#define _AT_DISK_DRIVER_H

#include "types.h"

/* sector size */
#define HD_SECTOR_SIZE 512

void hd_driver_init();
void disk_read_sector(uint32 sector, uint8* buf);

#endif
