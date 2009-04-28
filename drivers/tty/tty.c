#include "tty.h"
#include "gos.h"
#include "types.h"

PRIVATE int init(){
  
}

PRIVATE int read(void *buf, uint32 num_bytes){

}

PRIVATE int write(void *buf, uint32 num_bytes){

}

driver * tty_get_driver(){
  driver *d = (driver*)kmalloc(sizeof(driver));
  d->init = init;
  d->read = read;
  d->write = write;
}
