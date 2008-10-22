#include "kmalloc.h"
#include "types.h"
#include "gos.h"
#include "vm.h"
#include "mm.h"

void * kmalloc(uint32 size){
  uint32 pages_to_alloc = size / 4096 + 1;
  void *phys_mem;
  void *virt_mem;

  phys_mem = alloc_pages(pages_to_alloc);

  virt_mem = vm_alloc_at(phys_mem, (uint32)kern_phys_to_virt(phys_mem), size, USER);

  return virt_mem;
}
