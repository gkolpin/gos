#ifndef _FILESYSTEM_H
#define _FILESYSTEM_H

typedef struct filesystem {
  void * (*open)(const char *path);
  int (*read)(void *fs_data, void *buf, uint32 num_bytes);
  int (*write)(void *fs_data, void *buf, uint32 num_bytes);
  int (*close)(void *fs_data);
} filesystem;

#endif
