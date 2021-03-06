#include "kmalloc.h"
#include "types.h"
#include "gos.h"
#include "vm.h"
#include "mm.h"
#include "utility.h"
#include "kprintf.h"

#define NUM_OBJ_CACHES 9

typedef struct object_cache {
  uint32 	size;
  PAGE		*headPage;
} object_cache;

typedef struct alloc_record {
  void 		*virt_loc;
  uint		size;
  struct alloc_record	*next;
  struct alloc_record	*prev;
} alloc_record;

PRIVATE object_cache objectCaches[NUM_OBJ_CACHES];
PRIVATE uint32 max_size_obj;
PRIVATE alloc_record *alloc_records = NULL;

/* number of slots in a slab (page) */
#define PAGE_NO_SLOTS(obj_size) ((PAGE_SIZE / obj_size) - \
				(PAGE_SIZE / obj_size / BITS_PER_BYTE / obj_size) + \
				1)
/* gets the object pointer for the specified index on the slab (page) */
#define SLOT_DATA(page_data, idx_size, index, obj_size) ((void*)&((byte_t*)((uint32)page_data + \
								idx_size * sizeof(uint32)))[index * obj_size])
/* gets the index location of the specified object pointer */
#define INDEX_LOC(page_data, idx_size, object, obj_size) (((uint32)(object) - (uint32)(page_data) - \
							   (idx_size * sizeof(uint32))) / (obj_size))
/* returns the size of the index in number of 32 bit entities */
#define INDEX_SIZE(obj_size) ((PAGE_SIZE / obj_size / BITS_PER_UINT32) + \
				((PAGE_SIZE / obj_size) % BITS_PER_UINT32 ? \
				1 : 0))

PRIVATE void * get_obj_from_cache(uint32);
PRIVATE void * map_phys_to_kernel_mem(void*, uint32 no_pages);
PRIVATE void record_allocation(void *, uint32 no_bytes);
PRIVATE void * add_page(object_cache*);
PRIVATE void init_page(PAGE*, PAGE *next_page, PAGE *prev_page);
PRIVATE void * alloc_free_obj(object_cache*);
PRIVATE void * alloc_free_obj_in_page(PAGE *page, uint32 size);
PRIVATE void * get_real_page(PAGE *page);
PRIVATE void init_page_index(PAGE *page, uint32 obj_size);
PRIVATE bool kfree_large(void*);
PRIVATE bool kfree_small(void*);

void kmalloc_init(){
  int i;
  object_cache *curCache;
  for (i = 0; i < NUM_OBJ_CACHES; i++){
    curCache = &objectCaches[i];
    curCache->size = 4 << i;
    max_size_obj = curCache->size;
    curCache->headPage = NULL;
  }
}

void * kmalloc(uint32 size){
  void *virt_loc = NULL;
  void *phys_loc;

  if (size <= max_size_obj){
    /* allocation can be satisfied from one of the caches */
    kprintf("smaller than max_obj_size\n");
    virt_loc = get_obj_from_cache(size);
  } else {
    /* need to call alloc_pages directly */
    kprintf("allocating large object\n");
    phys_loc = alloc_pages(PAGES_FOR_BYTES(size));
    virt_loc = map_phys_to_kernel_mem(phys_loc, PAGES_FOR_BYTES(size));
    record_allocation(virt_loc, size);
  }

  kprintf("kmalloc: %u %u\n", size, (uintptr_t)virt_loc);
  return virt_loc;
}

PRIVATE void record_allocation(void *virt_loc, uint size){
  alloc_record *record = (alloc_record*)kmalloc(sizeof(alloc_record));
  alloc_record *curRecord;

  record->virt_loc = virt_loc;
  record->size = size;
  record->next = NULL;
  record->prev = NULL;

  if (alloc_records == NULL){
    alloc_records = record;
  } else {
    for (curRecord = alloc_records; curRecord->next != NULL; curRecord = curRecord->next);
    curRecord->next = record;
    record->prev = curRecord;
  }
}

void kfree(void *obj){
  bool freed;
  freed = kfree_small(obj);
  if (!freed){
    kfree_large(obj);
  }
  kprintf("kfree: %u\n", (uintptr_t)obj);
}

PRIVATE bool kfree_large(void *obj){
  alloc_record *curRecord;
  bool retVal = FALSE;

  for (curRecord = alloc_records; curRecord != NULL; curRecord = curRecord->next){
    if (obj >= curRecord->virt_loc && (uint32)obj < ((uint32)curRecord->virt_loc + curRecord->size)){
      retVal = TRUE;
      free_pages(kern_virt_to_phys(obj), PAGES_FOR_BYTES(curRecord->size));
      break;
    }
  }

  if (curRecord->prev != NULL){
    curRecord->prev->next = curRecord->next;
  }
  if (curRecord->next != NULL){
    curRecord->next->prev = curRecord->prev;
  }

  if (alloc_records == curRecord){
    alloc_records = curRecord->next;
  }

  kfree(curRecord);

  return retVal;
}

PRIVATE bool kfree_small(void *obj){
  /* search for page that contains the object */
  int i;
  object_cache *curCache = NULL;
  int indexLoc;
  uint32 *idx;
  PAGE *curPage;
  void *physPage;
  bool foundObj = FALSE;

  for (i = 0; i < NUM_OBJ_CACHES; i++){
    curCache = &objectCaches[i];

    for (curPage = curCache->headPage; curPage != NULL; curPage = curPage->next_slab){
      physPage = get_real_page(curPage);
      if (obj >= physPage && (uint32)obj < (uint32)physPage + PAGE_SIZE){
	indexLoc = INDEX_LOC(physPage, INDEX_SIZE(curCache->size), obj, curCache->size);
	kprintf("indexLoc: %d\n", indexLoc);
	idx = physPage;
	idx[indexLoc / BITS_PER_UINT32] ^= 
	  (1 << (indexLoc % BITS_PER_UINT32));
	foundObj = TRUE;
	goto END_LOOP;
      }
    }
  }

  if (!foundObj){
    return foundObj;
  }

 END_LOOP:
  /* loop through index to see if page can be freed */
  if (curCache != NULL && curPage != NULL){
    for (i = 0; i < INDEX_SIZE(curCache->size); i++){
      if (idx[i] != ~(uint32)0){
	return foundObj;
      }
    }
  }

  /* if we get here, our page is empty */
  physPage = kern_virt_to_phys(physPage);
  free_pages(physPage, 1);

  return foundObj;
}

PRIVATE void init_page(PAGE *page, PAGE *next_page, PAGE *prev_page){
  page->next_slab = next_page;
  page->prev_slab = prev_page;
}

PRIVATE void * map_phys_to_kernel_mem(void *phys_mem, uint32 pages){
  return vm_alloc_at(phys_mem, (uint32)kern_phys_to_virt(phys_mem), pages * PAGE_SIZE,
		     SUPERVISOR);
}

/*PRIVATE void record_allocation(void *virt_loc, uint32 size){
  
}*/

/* size is in bytes */
PRIVATE object_cache * get_obj_cache_for_size(uint32 size){
  int i;
  for (i = 0; i < NUM_OBJ_CACHES; i++){
    if (size <= objectCaches[i].size){
      return &objectCaches[i];
    }
  }

  return NULL;
}

/* size is in bytes */
PRIVATE void * get_obj_from_cache(uint32 size){
  object_cache *cache = get_obj_cache_for_size(size);
  void *temp;

  if (cache->headPage == NULL){
    kprintf("headPage is null\n");
    add_page(cache);
    return alloc_free_obj(cache);
  } else {
    temp = alloc_free_obj(cache);
    if (temp == NULL){
      add_page(cache);
      return alloc_free_obj(cache);
    } else {
      return temp;
    }
  }
}

PRIVATE void * add_page(object_cache *cache){
  void *temp;
  PAGE *curPage;
  if (cache->headPage == NULL){
    temp = alloc_pages(1);
    map_phys_to_kernel_mem(temp, 1);
    cache->headPage = get_page_struct(temp);
    init_page(cache->headPage, NULL, NULL);
    init_page_index(cache->headPage, cache->size);
    return temp;
  } else {
    for (curPage = cache->headPage; curPage->next_slab != NULL; 
	 curPage = curPage->next_slab);
    temp = alloc_pages(1);
    map_phys_to_kernel_mem(temp, 1);
    curPage->next_slab = get_page_struct(temp);
    init_page(curPage->next_slab, NULL, curPage);
    init_page_index(curPage->next_slab, cache->size);
    return temp;
  }
}

PRIVATE void * get_real_page(PAGE *page){
  uint32 phys_addr = get_phys_address_for_page(page);
  //return map_phys_to_kernel_mem((void*)phys_addr, 1);
  return kern_phys_to_virt((void*)phys_addr);
}

PRIVATE void init_page_index(PAGE *page, uint32 obj_size){
  int i;
  void *page_data = get_real_page(page);
  for (i = 0; i < INDEX_SIZE(obj_size); i++){
    /* init page index bits to 1 - all free */
    ((uint32*)page_data)[i] = ~((uint32)0);
  }
}

PRIVATE void * alloc_free_obj(object_cache *cache){
  PAGE *curPage;
  void *obj = NULL;
  for (curPage = cache->headPage; curPage != NULL; curPage = curPage->next_slab){
    obj = alloc_free_obj_in_page(curPage, cache->size);
    if (obj == NULL){
      continue;
    } else {
      break;
    }
  }

  kprintf("obj: %d\n", (uint32)obj);

  return obj;
}

PRIVATE void * alloc_free_obj_in_page(PAGE *page, uint32 obj_size){
  void *page_data = get_real_page(page);
  uint32 *idx = (uint32*)page_data;
  int i;
  int free_loc;
  int index_size = INDEX_SIZE(obj_size);

  for (i = 0; i < index_size; i++){
    if (!idx[i]){
      continue;
    }

    free_loc = bit_scan_forward(idx[i]);
    free_loc += (i * BITS_PER_UINT32);

    if (free_loc > PAGE_NO_SLOTS(obj_size)){
      continue;
    }

    kprintf("free_loc: %d\n", free_loc);
    kprintf("uint32: %d\n", idx[i]);
    kprintf("i: %d\n", i);
    idx[i] ^= (1 << free_loc);
    return SLOT_DATA(page_data, index_size, free_loc, obj_size);
  }

  kprintf("returning null\n");
  return NULL;
}
