#ifndef _FILESYSTEM_H
#define _FILESYSTEM_H

typedef struct filesystem {
  int (*open)(const char *path);
  int (*read)(int fd, void *buf, uint32 num_bytes);
  int (*write)(int fd, void *buf, uint32 num_bytes);
  int (*close)(int fd);
} filesystem;

#endif
