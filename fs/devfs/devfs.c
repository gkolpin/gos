#include "devfs.h"
#include "gos.h"
#include "list.h"
#include "driver.h"
#include "tty.h"

struct _devfs_open_file {
  driver *d;
};

PRIVATE void * open(const char *path){
  driver *dr;
  struct _devfs_open_file *of;

  if (!strcmp(path, "tty0")){
    dr = tty_get_driver();
  } else {
    return NULL;
  }
  
  of = (struct _devfs_open_file*)kmalloc(sizeof(struct _devfs_open_file));
  of->d = dr;

  return of;
}

PRIVATE int read(void * fs_data, void *buf, uint32 num_bytes){
  struct _devfs_open_file *of = (struct _devfs_open_file*)fs_data;
  return of->d->read(buf, num_bytes);
}

PRIVATE int write(void * fs_data, void *buf, uint32 num_bytes){
  struct _devfs_open_file *of = (struct _devfs_open_file*)fs_data;
  return of->d->write(buf, num_bytes);
}

PRIVATE int close(void * fs_data){
  kfree(fs_data);
  return 0;
}

filesystem * devfs_init(){
  filesystem *retVal = (filesystem*)kmalloc(sizeof(filesystem));
  retVal->open = open;
  retVal->read = read;
  retVal->write = write;
  retVal->close = close;
  return retVal;
}
