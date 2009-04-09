#include "devfs.h"
#include "gos.h"

PRIVATE int open(const char *path){
  
}

PRIVATE int read(int fd, void *buf, uint32 num_bytes){

}

PRIVATE int write(int fd, void *buf, uint32 num_bytes){

}

PRIVATE int close(int fd){

}

filesystem * devfs_init(){
  filesystem *retVal = (filesystem*)kmalloc(sizeof(filesystem));
  retVal->open = open;
  retVal->read = read;
  retVal->write = write;
  retVal->close = close;
  return retVal;
}
