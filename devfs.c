#include "devfs.h"
#include "gos.h"
#include "list.h"
#include "driver.h"
#include "tty.h"

struct _devfs_open_file {
  int fd;
  driver *d;
  list_node l_node;
};

PRIVATE list open_files;

PRIVATE int get_next_avail_fd();

PRIVATE int open(const char *path){
  driver *dr;
  struct _devfs_open_file *of;

  if (!strcmp(path, "tty0")){
    dr = tty_get_driver();
  } else {
    return -1;
  }
  
  of = (struct _devfs_open_file*)kmalloc(sizeof(struct _devfs_open_file));
  of->fd = get_next_avail_fd();
  of->d = dr;
  return of->fd;
}

PRIVATE int get_next_avail_fd(){
  list_node *curNode;
  struct _devfs_open_file *curOpenFile;
  /* assume the open_files list is sorted on file descriptors */
  int fd = 0;
  for (curNode = list_head(open_files); curNode != NULL; 
       curNode = list_next(open_files, curNode)){
    curOpenFile = cur_obj(open_files, curNode, struct _devfs_open_file);
    if (curOpenFile->fd == fd) fd++;
    else return fd;
  }
}

PRIVATE int read(int fd, void *buf, uint32 num_bytes){

}

PRIVATE int write(int fd, void *buf, uint32 num_bytes){

}

PRIVATE int close(int fd){

}

filesystem * devfs_init(){
  open_files = list_init(struct _devfs_open_file, l_node);

  filesystem *retVal = (filesystem*)kmalloc(sizeof(filesystem));
  retVal->open = open;
  retVal->read = read;
  retVal->write = write;
  retVal->close = close;
  return retVal;
}
