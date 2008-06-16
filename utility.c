#include "utility.h"
#include "types.h"

void * kmemcpy(void *dest, const void *src, uint32 n){
  uint32 i;

  for (i = 0; i < n; i++){
    ((uint8*)dest)[i] = ((uint8*)src)[i];
  }

  return dest;
}

void bzero(void *buf, uint32 size){
  int i;
  for (i = 0; i < size; i++){
    ((uint8*)buf)[i] = 0;
  }
}
