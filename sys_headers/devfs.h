#ifndef _DEVFS_H
#define _DEVFS_H

#include "driver.h"
#include "filesystem.h"

void devfs_init();
void register_device(driver *d, char *name, void *dev_data);

enum FILE_DESCRIPTORS {
  KEYBOARD,
  CONSOLE,
  NUM_DEVICES
};

#endif
