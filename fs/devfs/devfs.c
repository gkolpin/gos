#include "devfs.h"
#include "gos.h"
#include "list.h"
#include "driver.h"
#include "tty.h"

struct _devfs_device {
  char *name;
  driver *d;
  void *dev_data;
  list_node node;
};

PRIVATE list devices;

PRIVATE void * open(const char *path){
  list_node *curNode;
  struct _devfs_device *curDev;
  for (curNode = list_head(devices); curNode != NULL; curNode = list_next(devices, curNode)){
    curDev = cur_obj(devices, curNode, struct _devfs_device);
    if (!strcmp(curDev->name, path)){
      return curDev;
    }
  }

  return NULL;
}

PRIVATE int read(void * fs_data, void *buf, uint32 num_bytes){
  struct _devfs_device *dev = (struct _devfs_device*)fs_data;
  return dev->d->read(buf, num_bytes, dev->dev_data);
}

PRIVATE int write(void * fs_data, void *buf, uint32 num_bytes){
  struct _devfs_device *dev = (struct _devfs_device*)fs_data;
  return dev->d->write(buf, num_bytes, dev->dev_data);
}

PRIVATE int close(void * fs_data){
  kfree(fs_data);
  return 0;
}

void register_device(driver *d, char *name, void *dev_data){
  struct _devfs_device *device = (struct _devfs_device*)kmalloc(sizeof(struct _devfs_device));
  device->name = name;
  device->d = d;
  device->dev_data = dev_data;
  list_add(devices, &(device->node));
}

void devfs_init(){
  devices = list_init(struct _devfs_device, node);

  filesystem *fs = (filesystem*)kmalloc(sizeof(filesystem));
  fs->open = open;
  fs->read = read;
  fs->write = write;
  fs->close = close;
  register_fs("devfs", fs);
}
