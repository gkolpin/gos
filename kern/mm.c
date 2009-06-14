#include "gos.h"
#include "types.h"
#include "mm.h"
#include "kprintf.h"

#define MEM_PAGE_SIZE 	4096

#define ERROR -1

typedef struct mem_area {
  struct mem_area 	*next;	/* used for placing pages in linked lists */
  struct mem_area	*prev;
  uint32		order;
  bool 			isFree;
  PAGE			page;
} mem_area;

PRIVATE uint32 max_order;

PRIVATE mem_area ** free_mem_areas;
PRIVATE mem_area * mem_area_slots;
uint32 no_slots;

#define IS_POW_2(n) (!((n) & ((n) - 1)))
#define POW_BASE_2(n) (1 << (n))
#define BUDDY_PAGE(start_page, order) ((start_page) ^ (POW_BASE_2(order)))
#define START_PAGE(ma) (((uint32)(ma) - (uint32)mem_area_slots) / sizeof(mem_area))

PRIVATE void * get_phys_addr(mem_area*);
PRIVATE void split_mem_area_to(uint32 cur_order, uint32 requested_order);
PRIVATE uint32 order_of(uint32);
PRIVATE void free_area_add_to(mem_area*, uint32 order);
PRIVATE mem_area * get_slot_for_page(uint32 page_no);
PRIVATE void coalesce(mem_area*, uint32 order);
PRIVATE void remove_from_free_list(mem_area*, uint32 order);

void * alloc_pages(uint32 no_pages){
  uint32 order = order_of(no_pages);
  uint32 cur_order;
  mem_area *maTemp;
  
  if (free_mem_areas[order] != NULL){
    maTemp = free_mem_areas[order];
    remove_from_free_list(maTemp, order);
    kprintf("alloc_pages: %u %u\n", no_pages, get_phys_addr(maTemp));
    return get_phys_addr(maTemp);
  } else {
    cur_order = order;
    while (++cur_order <= max_order){
      if (free_mem_areas[cur_order] != NULL){
	split_mem_area_to(cur_order, order);
	maTemp = free_mem_areas[order];
	remove_from_free_list(maTemp, order);
	kprintf("alloc_pages: %u %u\n", no_pages, get_phys_addr(maTemp));
	return get_phys_addr(maTemp);
      } 
    }
  }

  return NULL;
}

void free_pages(void *start_address, uint32 no_pages){
  uint32 start_page_no = (uint32)start_address / MEM_PAGE_SIZE;
  mem_area *maTemp = get_slot_for_page(start_page_no);
  uint32 order = order_of(no_pages);
  no_pages = POW_BASE_2(order);

  free_area_add_to(maTemp, order);

  coalesce(maTemp, order);
  kprintf("free_pages: %u %u\n", no_pages, start_address);
}

uint32 get_phys_address_for_page(PAGE *page){
  /* first get the mem_area */
  uint32 ma_addr = (uint32)page - (uint32)(&((mem_area*)0)->page);
  return (uint32)get_phys_addr((mem_area*)ma_addr);
}

PAGE * get_page_struct(void *phys_mem){
  mem_area *ma = get_slot_for_page(((uint32)phys_mem) / MEM_PAGE_SIZE);
  return &ma->page;
}

PRIVATE void coalesce(mem_area *ma, uint32 order){
  uint32 buddy_page = BUDDY_PAGE(START_PAGE(ma), order);
  mem_area *maBuddy = &mem_area_slots[buddy_page];

  if (buddy_page >= no_slots){
    return;
  }

  if (order == max_order){
    return;
  }

  if (maBuddy->order == ma->order && maBuddy->isFree){
    remove_from_free_list(maBuddy, order);
    remove_from_free_list(ma, order);

    free_area_add_to(ma, order + 1);

    coalesce(ma, order + 1);
  }
}

PRIVATE void remove_from_free_list(mem_area *ma, uint32 order){
  mem_area *prev, *next;

  if (ma->next == ma){
    free_mem_areas[order] = NULL;
  } else {
    prev = ma->prev;
    next = ma->next;

    prev->next = next;
    next->prev = prev;

    if (free_mem_areas[order] == ma){
      free_mem_areas[order] = next;
    }
  }

  ma->prev = NULL;
  ma->next = NULL;
  ma->isFree = FALSE;
}

PRIVATE void * get_phys_addr(mem_area *slot){
  return (void*)(START_PAGE(slot) * MEM_PAGE_SIZE);
}

PRIVATE uint32 order_of(uint32 n){
  uint32 order = 0;
  uint32 ui_temp = 1;
  while (ui_temp < n){
    order++;
    ui_temp <<= 1;
  }
  return order;
}

PRIVATE void split_mem_area_to(uint32 cur_order, uint32 requested_order){
  mem_area *maTemp;

  if (cur_order == requested_order){
    return;
  }
  
  maTemp = free_mem_areas[cur_order];
  /* remove this mem_area from the list */
  remove_from_free_list(maTemp, cur_order);
  /* split maTemp and */
  /* place both pieces in free area one order down */
  free_area_add_to(maTemp, cur_order - 1);
  free_area_add_to(&mem_area_slots[BUDDY_PAGE(START_PAGE(maTemp), cur_order - 1)], 
		   cur_order - 1);

  split_mem_area_to(cur_order - 1, requested_order);
}

/* free area lists are circular doubly linked lists */
PRIVATE void free_area_add_to(mem_area *maFree, uint32 order){
  mem_area **maTemp;

  maTemp = &free_mem_areas[order];
  if (*maTemp == NULL){
    *maTemp = maFree;
    maFree->next = maFree;
    maFree->prev = maFree;
  } else {
    maFree->next = (*maTemp)->next;
    maFree->next->prev = maFree;
    (*maTemp)->next = maFree;
    maFree->prev = *maTemp;
  }

  maFree->isFree = TRUE;
  maFree->order = order;
}

PRIVATE mem_area * get_slot_for_page(uint32 page_no){
  return &mem_area_slots[page_no];
}

void mm_init(mem_map *mm, uint32 *start_page_reserved, uint32 *no_pages_reserved){
  mem_area *maTemp;
  mem_map *mmTemp;
  uint32 reserved_pages = 0;
  uint32 total_pages = 0;
  uint32 free_mem_area_array_space;
  uint32 uiTemp, uiTemp2;

  /* set max order */
  for (mmTemp = mm; mmTemp != NULL; mmTemp = mmTemp->next){
    if (mmTemp->reserved){
      continue;
    }
    if (IS_POW_2(mmTemp->pages) && mmTemp->pages > max_order){
      max_order = order_of(mmTemp->pages) - 1;
    } else if (!IS_POW_2(mmTemp->pages)) {
      if ((order_of(mmTemp->pages) - 1) > max_order){
	max_order = order_of(mmTemp->pages) - 1;
      }
    }
  }

  /* get total pages */
  for (mmTemp = mm; mmTemp != NULL; mmTemp = mmTemp->next){
    total_pages += mmTemp->pages;
  }

  no_slots = total_pages;

  free_mem_area_array_space = ((max_order + 1) * sizeof(mem_area*)) / MEM_PAGE_SIZE + 1;

  reserved_pages = ((total_pages * sizeof(mem_area)) / MEM_PAGE_SIZE) + 1 + free_mem_area_array_space;

  kprintf("num reserved pages: %d\n", reserved_pages);

  /* -find mem map area with enough pages for page catalog */
  /* -we assume that there is a mem map node that contains enough space */
  for (mmTemp = mm; mmTemp != NULL; mmTemp = mmTemp->next){
    if (mmTemp->reserved){
      continue;
    }
    if (mmTemp->pages >= reserved_pages){
      (*start_page_reserved) = mmTemp->page_loc;
      (*no_pages_reserved) = reserved_pages;

      kprintf("beginning mmap loc: %d\n", mmTemp->page_loc * MEM_PAGE_SIZE);
      kprintf("mmap no_pages: %d\n", mmTemp->pages);
      mmTemp->pages -= reserved_pages;
      free_mem_areas = (mem_area**)(mmTemp->page_loc * MEM_PAGE_SIZE);
      mem_area_slots = (mem_area*)((uint32)free_mem_areas + free_mem_area_array_space * MEM_PAGE_SIZE);
      mmTemp->page_loc += reserved_pages;
      kprintf("end mmap loc: %d\n", mmTemp->page_loc * MEM_PAGE_SIZE);
      break;
    }
  }

  for (uiTemp = 0; uiTemp < no_slots; uiTemp++){
    mem_area_slots[uiTemp].isFree = FALSE;
  }

  /* setup free mem areas */
  for (mmTemp = mm; mmTemp != NULL; ){
    
    if (mmTemp->pages == 0 || 
	mmTemp->reserved){
      mmTemp = mmTemp->next;
      continue;
    }

    if (IS_POW_2(mmTemp->pages)){
      maTemp = get_slot_for_page(mmTemp->page_loc);
      free_area_add_to(maTemp, order_of(mmTemp->pages));
      mmTemp = mmTemp->next;
    } else {
      uiTemp = order_of(mmTemp->pages) - 1;
      uiTemp2 = POW_BASE_2(uiTemp);
      maTemp = get_slot_for_page(mmTemp->page_loc);
      free_area_add_to(maTemp, uiTemp);

      mmTemp->pages -= uiTemp2;
      mmTemp->page_loc += uiTemp2;
    }
  }

  kprintf("mem_area_slots: %d\n", mem_area_slots);
  kprintf("mem_area_slots end: %d\n", &mem_area_slots[no_slots]);
  kprintf("no slots: %d\n", no_slots);
  kprintf("mem_area_slots_space: %d\n", no_slots * sizeof(mem_area));
}
