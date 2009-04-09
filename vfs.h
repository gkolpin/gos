#ifndef _VFS_H
#define _VFS_H

#include "types.h"
#include "filesystem.h"

typedef struct _vfd *vfd;

int vfs_init();
vfd vfs_open(const char *path);
int vfs_read(vfd fd, void *buf, uint32 num_bytes);
int vfs_write(vfd fd, void *buf, uint32 num_bytes);
int vfs_close(vfd fd);
int vfs_mount(const char *type, const char *dir);

void register_fs(const char *type, filesystem*);

#endif
