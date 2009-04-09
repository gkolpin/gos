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

void kmemcpyphys(uint32 page_dest, uint32 page_src, uint32 no_pages){
  void * virt_dest = vm_alloc_at((void*)(page_dest * PAGE_SIZE),
				 (uint32)kern_phys_to_virt((void*)(page_dest * PAGE_SIZE)),
				 no_pages * PAGE_SIZE, SUPERVISOR);
  void * virt_src = vm_alloc_at((void*)(page_src * PAGE_SIZE),
				(uint32)kern_phys_to_virt((void*)(page_src * PAGE_SIZE)),
				no_pages * PAGE_SIZE, SUPERVISOR);

  kmemcpy(virt_dest, virt_src, no_pages * PAGE_SIZE);
}

void bzero(void *buf, uint32 size){
  int i;
  for (i = 0; i < size; i++){
    ((uint8*)buf)[i] = 0;
  }
}

int strcmp(const char *s1, const char *s2){
  return strncmp(s1, s2, SIZE_T_MAX);
}

int strncmp(const char *s1, const char *s2, size_t len){
  for (; *s1 && *s2 && len; s1++, s2++, len--){
    if (*s1 > *s2) return 1;
    if (*s1 < *s2) return -1;
  }
  if (!*s1 && *s2) return 1;
  if (!*s2 && *s1) return -1;
  return 0;  
}

size_t strlen(const char *s){
  int len = 0;
  while (*(s++)) len++;
  return len;
}
