#include <stdio.h>
#include <string.h>

typedef struct {
  unsigned int loc, size;
} chunk;

chunk chunks[500];
int numChunks = 0;

void _free(unsigned int loc){
  int i, j;
  for (i = 0; i < numChunks; i++){
    if (chunks[i].loc == loc){
      for (j = i + 1; j < numChunks; i++, j++){
	chunks[i] = chunks[j];
      }
      numChunks--;
      return;
    }
  }
  
  puts("FREE ERROR!!\n");
}

void _free2(unsigned int loc, unsigned int size){
  int i, j;
  for (i = 0; i < numChunks; i++){
    if (chunks[i].loc == loc){

      if (chunks[i].size != size){
	puts("SIZE DISCREPANCY!!");
      }

      for (j = i + 1; j < numChunks; i++, j++){
	chunks[i] = chunks[j];
      }

      numChunks--;
      return;
    }
  }
  
  puts("FREE ERROR!!\n");
}

void check_safety(){
  /* make sure two allocations don't share same location */
  unsigned int start1, end1, start2, end2;
  int i, j;
  for (i = 0; i < numChunks; i++){
    for (j = i + 1; j < numChunks; j++){
      start1 = chunks[i].loc;
      end1 = start1 + chunks[i].size;
      start2 = chunks[j].loc;
      end2 = start2 + chunks[j].size;
      if (!(end1 <= start2 || end2 <= start1)){
	puts("SAFETY ERROR 1");
	printf("%u %u %u %u\n", start1, end1, start2, end2);
      }
    }
  }
}

void _malloc(unsigned int loc, unsigned int size){
  chunk *curChunk;
  curChunk = &chunks[numChunks];
  curChunk->loc = loc;
  curChunk->size = size;
  numChunks++;

  check_safety();
}

int main(){
  char line[100];
  char *line_ptr;
  unsigned int bytes, loc;
  while (NULL != fgets(line, 100, stdin)){
    line_ptr = line;

    if (strstr(line, "kfree:")){
      line_ptr += sizeof("kfree:");
      sscanf(line_ptr, "%u", &loc);
      _free(loc);
    } else if (strstr(line, "kmalloc:")){
      line_ptr += sizeof("kmalloc:");
      sscanf(line_ptr, "%u %u", &bytes, &loc);
      _malloc(loc, bytes);
    } else if (strstr(line, "alloc_pages:")){
      line_ptr += sizeof("alloc_pages:");
      sscanf(line_ptr, "%u %u", &bytes, &loc);
      _malloc(loc, bytes * 4096);
    } else if (strstr(line, "free_pages:")){
      line_ptr += sizeof("free_pages:");
      sscanf(line_ptr, "%u %u", &bytes, &loc);
      _free2(loc, bytes * 4096);
    }
  }

  return 0;
}
