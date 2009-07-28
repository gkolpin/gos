#include "vm.h"
#include "gos.h"
#include "types.h"
#include "mm.h"
#include "utility.h"
#include "kprintf.h"

/* number of page directory and table entries */
#define NUM_ENTRIES (PAGE_SIZE / sizeof(reg_t))
/* amount of vm reserved for kernel - 1MB */
#define KERNEL_THRESHOLD 0x100000
/* supervisor, write flags for page directory entry */
#define PDE_SW 0x1B
/* user, write flags */
#define PDE_UW 0x1F
/* user, read-only flags */
#define PDE_UR 0x1D
/* flags mask */
#define FLAGS_MASK 0xFFF
/* page dir entry mask */
#define PAGE_DIR_ENTRY_MASK 0xFFC00000
/* page table entry mask */
#define PAGE_TABLE_ENTRY_MASK 0x3FF000
/* page directory bit shift amnt */
#define PAGE_DIR_BIT_SHIFT 22
/* page table bit shift amnt */
#define PAGE_TABLE_BIT_SHIFT 12
/* empty page entry */
#define EMPTY_PAGE 0
/* user mem starts at 1MB */
#define USER_HEAP_START KERNEL_THRESHOLD
/* vm map starts at 3GB - slot 768 */
#define KERN_VM_DIR_ENTRY 768
/* vm map size - 4MB for page tables plus 1KB for page dir */
#define VM_MAP_SIZE ((1 << 22) + 0x1000)
/* page directory virtual memory location */
#define PAGE_DIR_VM_LOC ((uint)0 - 4096)
/* pt_map start address - 4MB from end of address space */
#define PT_MAP_ADDR ((uint32)0 - ((uint32)1 << 22))

typedef uint32 entry_t;
typedef entry_t* PAGE_DIR;

PRIVATE PAGE_DIR page_dir;
PRIVATE PAGE_DIR *pt_map;
/* start of free space on kernel heap */
/*PRIVATE uint32 kern_heap_free_addr = KERNEL_HEAP_START + VM_MAP_SIZE;*/
/* start of free space on user heap */
/*PRIVATE uint32 user_heap_free_addr = USER_HEAP_START;*/
/* kernel virtual mem offset to translate between phys and virt mem */
PRIVATE uint32 kern_virt_mem_offset = 0;

PRIVATE bool paging_enabled = FALSE;

#define VIRT2PDE(virt_mem) ((uint32)virt_mem >> PAGE_DIR_BIT_SHIFT)
#define VIRT2PTE(virt_mem) (((uint32)virt_mem & PAGE_TABLE_ENTRY_MASK) \
 				>> PAGE_TABLE_BIT_SHIFT)

/* defined in i386lib.s */
void load_cr3(uint32);
void enable_paging();

PRIVATE entry_t create_entry(uint32 loc, uint32 flags);
PRIVATE void create_table_entries(uint32 page_dir_index, uint32 page_table_index,
				  uint32 physical_page_no, uint32 flags);
PRIVATE void map_pages(uint32 virtual_page_no, uint32 physical_page_no, 
		       uint32 no_pages, uint32 flags);
PRIVATE uint32 entry_page_no(entry_t);
PRIVATE void print_page_table();

void vm_init(uint32 start_reserved_kernel_page, uint32 no_reserved_kernel_pages){
  int i;
  
  /* allocate one page for the page directory */
  /* alloc_pages will always give page-aligned memory */
  page_dir = (PAGE_DIR)alloc_pages(1);
  bzero(page_dir, PAGE_SIZE);
  load_cr3((uint32)page_dir);

  /* map the page dir onto itself - after doing this  */
  /* we'll be able to reference the page directory by  */
  /* pointing to the top KB of mem. */
  page_dir[NUM_ENTRIES - 1] = create_entry((uint32)page_dir / PAGE_SIZE, PDE_UR);
  
  /* set up pt_map */
  pt_map = (PAGE_DIR*)alloc_pages(1);
  for (i = 0; i < NUM_ENTRIES - 1; i++){
    pt_map[i] = (PAGE_DIR)(PT_MAP_ADDR + i * PAGE_SIZE);
  }

  map_pages((KERNEL_HEAP_START + (uint32)pt_map) / PAGE_SIZE, 
	    (uint32)pt_map / PAGE_SIZE,
	    1, PDE_SW);

  /* identity map first MB of pages for kernel */
  map_pages(0, 0, KERNEL_THRESHOLD / PAGE_SIZE, PDE_SW);

  map_pages(start_reserved_kernel_page,
	    start_reserved_kernel_page,
	    no_reserved_kernel_pages, PDE_SW);

  print_page_table();

  /* set bit 31 of CR0 register to enable paging */
  enable_paging();

  paging_enabled = TRUE;
  kern_virt_mem_offset = KERNEL_HEAP_START;
  page_dir = (PAGE_DIR)PAGE_DIR_VM_LOC;
  pt_map = (PAGE_DIR*)kern_phys_to_virt(pt_map);
}

/*uint32 kern_heap_virt_addr_start(){
  return kern_heap_free_addr;
  }*/

void * kern_phys_to_virt(void *phys_addr){
  return (void*)((uint32)phys_addr + kern_virt_mem_offset);
}

void * kern_virt_to_phys(void *virt_addr){
  return (void*)((uint32)virt_addr - kern_virt_mem_offset);
}

void * kmemmap2virt(void *phys_addr, uint32 no_pages){
  return vm_alloc_at(phys_addr, (uint32)kern_phys_to_virt(phys_addr),
		     no_pages * PAGE_SIZE, SUPERVISOR);
}

void * virt2phys(uint32 pd_phys_addr, void *virt_src){
  PAGE_DIR pd = (PAGE_DIR)kmemmap2virt((void*)pd_phys_addr, 1);
  entry_t pt_entry = pd[VIRT2PDE(virt_src)];
  PAGE_DIR pt = (PAGE_DIR)kmemmap2virt((void*)(entry_page_no(pt_entry) * PAGE_SIZE), 1);
  uint32 phys_page = entry_page_no(pt[VIRT2PTE(virt_src)]);
  uint32 phys_loc = phys_page * PAGE_SIZE + (uint32)virt_src % PAGE_SIZE;
  return (void*)(phys_loc);
}

uint32 copy_cur_page_dir(){
  int i;
  PAGE_DIR copy = (PAGE_DIR)alloc_pages(1);
  kmemcpy2phys(copy, (void*)PAGE_DIR_VM_LOC, PAGE_SIZE);
  copy = kmemmap2virt(copy, 1);
  copy[NUM_ENTRIES - 1] = create_entry((uint32)kern_virt_to_phys(copy) / PAGE_SIZE, PDE_UR);
  /* start at 1 because first 4MB should stay the same for now */
  for (i = 1; i < KERN_VM_DIR_ENTRY; i++){
    copy[i] = (entry_t)0;
  }
  return (uint32)kern_virt_to_phys(copy);
}

void pd_free(uint32 pd_phys_addr){
  PAGE_DIR pd = kmemmap2virt((void*)pd_phys_addr, 1);
  int i;

  for (i = 1; i < KERN_VM_DIR_ENTRY; i++){
    if (pd[i] != EMPTY_PAGE){
      free_pages((void*)(entry_page_no(pd[i]) * PAGE_SIZE), 1);
    }
  }

  free_pages((void*)pd_phys_addr, 1);
}

void set_pd(uint32 pd_phys){
  load_cr3(pd_phys);
}

PRIVATE void map_pages(uint32 virtual_page_no, uint32 physical_page_no, 
		       uint32 no_pages, uint32 flags){
  uint32 pd_entry_start = (((virtual_page_no * PAGE_SIZE) & 
			    PAGE_DIR_ENTRY_MASK) >> PAGE_DIR_BIT_SHIFT);
  uint32 pt_entry_start = (((virtual_page_no * PAGE_SIZE) & 
			    PAGE_TABLE_ENTRY_MASK) >> PAGE_TABLE_BIT_SHIFT);

  uint32 end_page = virtual_page_no + no_pages;

  uint32 num_pd_entries = (((end_page * PAGE_SIZE) & 
			    PAGE_DIR_ENTRY_MASK) >> PAGE_DIR_BIT_SHIFT);
  uint32 num_pt_entries = (((end_page * PAGE_SIZE) & 
			    PAGE_TABLE_ENTRY_MASK) >> PAGE_TABLE_BIT_SHIFT);

  uint32 pd_index, pt_index;

  for (pd_index = pd_entry_start; pd_index <= num_pd_entries; pd_index++){
    for (pt_index = pt_entry_start; 
	 pt_index < (pd_index == num_pd_entries ? num_pt_entries : NUM_ENTRIES);
	 pt_index++, physical_page_no++){
      create_table_entries(pd_index, pt_index, physical_page_no, flags);
    }
  }
}

PRIVATE PAGE_DIR allocate_new_pt(uint32 page_dir_index){
  uint32 newpt_page_no;

  newpt_page_no = ((uint32)alloc_pages(1)) >> PAGE_TABLE_BIT_SHIFT;
  page_dir[page_dir_index] = create_entry(newpt_page_no, PDE_UW);

  if (paging_enabled){
    return pt_map[page_dir_index];
  } else {
    return (PAGE_DIR)(newpt_page_no << PAGE_TABLE_BIT_SHIFT);
  }
}

PRIVATE void create_table_entries(uint32 page_dir_index, uint32 page_table_index,
				  uint32 physical_page_no, uint32 flags){
  PAGE_DIR new_pt;

  if (page_dir[page_dir_index] == EMPTY_PAGE){
    new_pt = allocate_new_pt(page_dir_index);
    bzero(new_pt, PAGE_SIZE);
    new_pt[page_table_index] = create_entry(physical_page_no, flags);
  } else {
    if (paging_enabled){
      pt_map[page_dir_index][page_table_index] = create_entry(physical_page_no, flags);
    } else {
      new_pt = (PAGE_DIR)(entry_page_no(page_dir[page_dir_index]) << PAGE_TABLE_BIT_SHIFT);
      new_pt[page_table_index] = create_entry(physical_page_no, flags);
    }
  }
}

PRIVATE uint32 entry_page_no(entry_t entry){
  return entry >> PAGE_TABLE_BIT_SHIFT;
}

PRIVATE entry_t create_entry(uint32 loc, uint32 flags){
  loc <<= 12;
  loc |= (flags & FLAGS_MASK);
  return (entry_t)loc;
}

void * vm_alloc_at(void * phys_addr, uint32 requested_loc, uint32 size, user_type u_type){
  //void* phys_mem = (void*)malloc(size);

  uint32 privilege_flags;
  if (u_type == SUPERVISOR){
    privilege_flags = PDE_SW;
  }
  else if (u_type == USER){
    privilege_flags = PDE_UW;
  }

  map_pages(requested_loc / PAGE_SIZE, ((uint32)phys_addr) / PAGE_SIZE,
	    size % PAGE_SIZE == 0 ? size / PAGE_SIZE : size / PAGE_SIZE + 1,
	    privilege_flags);

  return (void*)requested_loc;
}

void print_page_table(){
  int pdIndex, ptIndex;
  uint32 ptPageLoc;
  PAGE_DIR curPageTable;
  bool started = FALSE;
  uint32 virtMemStart, virtMemEnd, physMemStart, physMemEnd;
  kprintf("**** start page table output ****\n");
  for (pdIndex = 0; pdIndex < 1024; pdIndex++){
    if (page_dir[pdIndex] != 0){
      ptPageLoc = entry_page_no(page_dir[pdIndex]);
      ptPageLoc <<= 12;
      curPageTable = (PAGE_DIR)kern_phys_to_virt((void*)ptPageLoc);
      for (ptIndex = 0; ptIndex < 1024; ptIndex++){
	if (curPageTable[ptIndex] != 0 && !started){
	  started = TRUE;
	  virtMemStart = pdIndex * (1 << 22) + ptIndex * (1 << 12);
	  physMemStart = entry_page_no(curPageTable[ptIndex]) << 12;
	} else if (curPageTable[ptIndex] == 0 && started){
	  started = FALSE;
	  virtMemEnd = pdIndex * (1 << 22) + ptIndex * (1 << 12) - 1;
	  physMemEnd = physMemStart + (virtMemEnd - virtMemStart);
	  kprintf("%x - %x  ->  %x - %x\n", virtMemStart, virtMemEnd, 
		  physMemStart, physMemEnd);
	}
      }
    } 
  }
  kprintf("**** end page table ****\n");
}

/*void * vm_malloc(uint32 size, user_type u_type){
  void *return_val;

  if (u_type == SUPERVISOR){
    return_val = vm_malloc_at(kern_heap_free_addr, size, u_type);
    kern_heap_free_addr += (size % PAGE_SIZE == 0 ? size / PAGE_SIZE : size / PAGE_SIZE + 1);
  }
  else {
    return_val = vm_malloc_at(user_heap_free_addr, size, u_type);
    user_heap_free_addr += (size % PAGE_SIZE == 0 ? size / PAGE_SIZE : size / PAGE_SIZE + 1);
  }

  return return_val;
}
*/
