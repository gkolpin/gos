#include "gos.h"
#include "types.h"
#include "elf.h"
#include "kprintf.h"
#include "task.h"
#include "vm.h"
#include "mm.h"
#include "utility.h"

task * create_task_from_elf(byte_t *fileData, uint32 no_bytes){
  Elf32_Ehdr *elf_header = (Elf32_Ehdr*)fileData;
  Elf32_Phdr *curHeader;
  task * retTask = create_task(elf_header->e_entry);
  void *segPhysMem;
  void *virtAddrTemp;

  int i;
  for (i = 0; i < elf_header->e_phnum; i++){
    curHeader = (Elf32_Phdr*)&fileData[elf_header->e_phoff + i * elf_header->e_phentsize];
    
    segPhysMem = alloc_pages(PAGES_FOR_BYTES(curHeader->p_memsz));
    kmemcpy2phys(segPhysMem, (void*)&fileData[curHeader->p_offset], 
		  curHeader->p_filesz);

    /* zero out uninitialized data */
    if (curHeader->p_memsz > curHeader->p_filesz){
      virtAddrTemp = kmemmap2virt(segPhysMem, PAGES_FOR_BYTES(curHeader->p_memsz));
      bzero(((uint8*)virtAddrTemp + curHeader->p_filesz), 0);
    }

    add_task_segment(retTask, (uint32)segPhysMem,
		     curHeader->p_filesz, curHeader->p_vaddr, 
		     PAGES_FOR_BYTES(curHeader->p_memsz));
  }

  return retTask;
}
