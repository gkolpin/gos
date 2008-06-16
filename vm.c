#include "vm.h"
#include "gos.h"
#include "types.h"

/* number of page directory and table entries */
#define NUM_ENTRIES (PAGE_SIZE / sizeof(reg_t))
/* amount of vm reserved for kernel - 1MB */
#define KERNEL_THRESHOLD 0x100000
/* supervisor, write flags for page directory entry */
#define PDE_SW 0x1B
/* user, write flags */
#define PDE_UW 0x1F
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

typedef uint32 entry_t;
typedef entry_t* PAGE_DIR;

PRIVATE PAGE_DIR page_dir;

/* defined in i386lib.s */
void load_cr3(uint32);
void enable_paging();

PRIVATE entry_t create_entry(uint32 loc, uint32 flags);
PRIVATE void create_table_entries(uint32 page_dir_index, uint32 page_table_index,
				  uint32 physical_page_no, uint32 flags);
PRIVATE void map_pages(uint32 virtual_page_no, uint32 physical_page_no, 
		       uint32 no_pages, uint32 flags);
PRIVATE uint32 entry_page_no(entry_t);

void vm_init(){
  /* set up page tables - initially, the kernel will be mapped to the
     first MB of virtual memory */
  
  /* allocate one page for the page directory */
  /* malloc will always give page-aligned memory */
  void *pdb = (void*)malloc(PAGE_SIZE);

  page_dir = (PAGE_DIR)pdb;
  bzero(pdb, PAGE_SIZE);
  load_cr3((uint32)pdb);

  /* map pag_dir into vm */
  map_pages((uint32)pdb / PAGE_SIZE, (uint32)pdb / PAGE_SIZE,
	    1, PDE_SW);

  /* identity map first MB of pages for kernel */
  map_pages(0, 0, KERNEL_THRESHOLD / PAGE_SIZE, PDE_SW);

  /* set bit 31 of CR0 register to enable paging */
  enable_paging();
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

  kprint_int(num_pd_entries);
  kprint_int(num_pt_entries);
  kprint_int(pd_entry_start);
  kprint_int(pt_entry_start);

  uint32 pd_index, pt_index;
  uint32 cur_physical_page = physical_page_no;

  for (pd_index = pd_entry_start; pd_index <= num_pd_entries; pd_index++){
    for (pt_index = pt_entry_start; 
	 pt_index < (pd_index == num_pd_entries ? num_pt_entries : NUM_ENTRIES);
	 pt_index++, physical_page_no++){
      create_table_entries(pd_index, pt_index, physical_page_no, flags);
    }
  }
}

PRIVATE void create_table_entries(uint32 page_dir_index, uint32 page_table_index,
				  uint32 physical_page_no, uint32 flags){
  uint32 new_page_addr;
  uint32 new_page_no;
  PAGE_DIR new_page_table;

  if (page_dir[page_dir_index] == EMPTY_PAGE){
    kprintf("allocating new page\n");

    new_page_addr = (uint32)malloc(PAGE_SIZE);
    kprint_int((uint32)new_page_addr);
    new_page_no = new_page_addr >> PAGE_TABLE_BIT_SHIFT;

    page_dir[page_dir_index] = create_entry(new_page_no, flags);
    map_pages((uint32)new_page_addr / PAGE_SIZE,
	      (uint32)new_page_addr / PAGE_SIZE, 1, PDE_SW);

    new_page_table = (PAGE_DIR)new_page_addr;
    /* make page DIRECTORY entries have user privilege level */
    new_page_table[page_table_index] = create_entry(physical_page_no, PDE_UW);  
  } else {
    new_page_table = (PAGE_DIR)(entry_page_no(((entry_t)page_dir[page_dir_index])) << PAGE_TABLE_BIT_SHIFT);
    new_page_table[page_table_index] = create_entry(physical_page_no, flags);
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

void * vm_malloc(uint32 requested_loc, uint32 size, user_type u_type){
  void* phys_mem = (void*)malloc(size);

  uint32 privelege_flags;
  if (u_type == SUPERVISOR){
    privelege_flags = PDE_SW;
  }
  else if (u_type == USER){
    privelege_flags = PDE_UW;
  }

  map_pages(requested_loc / PAGE_SIZE, ((uint32)phys_mem) / PAGE_SIZE,
	    size % PAGE_SIZE == 0 ? size / PAGE_SIZE : size / PAGE_SIZE + 1,
	    privelege_flags);

  return (void*)requested_loc;
}
