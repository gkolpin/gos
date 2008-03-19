#include "console.h"
#include "types.h"
#include "gos.h"

#define VGA_MEM_BASE 	0xB8000
/* Default color is grey on black */
#define DEFAULT_COLOR	0x7 

#define CONS_MAX_ROW 	25
#define CONS_MAX_COL	80

PRIVATE	ushort 	*cur_loc;
PRIVATE uint 	cur_loc_index;

enum {CHAR=0, COLOR};

void cons_init(void){
  cur_loc_index = 0;

  cur_loc = (ushort*)VGA_MEM_BASE;
}

void cons_putchar(char c){
  switch(c){
  case '\n':
    cur_loc_index += CONS_MAX_COL;
    cur_loc_index -= cur_loc_index % CONS_MAX_COL;
    break;
  case '\r':
    cur_loc_index -= cur_loc_index % CONS_MAX_COL;
    break;
  default:
    cur_loc[cur_loc_index] = DEFAULT_COLOR;
    cur_loc[cur_loc_index] <<= 8;
    cur_loc[cur_loc_index] |= c;
    cur_loc_index++;
    break;
  }


}
