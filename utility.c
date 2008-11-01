#include "utility.h"
#include "types.h"
#include "vm.h"

void * kmemcpy(void *dest, const void *src, uint32 n){
  uint32 i;

  for (i = 0; i < n; i++){
    ((uint8*)dest)[i] = ((uint8*)src)[i];
  }

  return dest;
}

void * kmemcpy2phys(void *phys_dest, const void *virt_src, uint32 size){
  void *virt_dest = vm_alloc_at(phys_dest, (uint32)kern_phys_to_virt(phys_dest),
				size, SUPERVISOR);

  kmemcpy(virt_dest, virt_src, size);

  return phys_dest;
}

void bzero(void *buf, uint32 size){
  int i;
  for (i = 0; i < size; i++){
    ((uint8*)buf)[i] = 0;
  }
}
