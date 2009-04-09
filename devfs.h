#ifndef _DEVFS_H
#define _DEVFS_H

#include "driver.h"
#include "filesystem.h"

filesystem * devfs_init();

enum FILE_DESCRIPTORS {
  KEYBOARD,
  CONSOLE,
  NUM_DEVICES
};

#endif
