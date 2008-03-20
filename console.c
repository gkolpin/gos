#include "console.h"
#include "types.h"
#include "gos.h"

#define VGA_MEM_BASE 	0xB8000
/* Default color is grey on black */
#define DEFAULT_COLOR	0x7 

#define CONS_MAX_ROW 	25
#define CONS_MAX_COL	80
#define CONS_SIZE	(CONS_MAX_COL * CONS_MAX_ROW)

PRIVATE	ushort 	*cur_loc;
PRIVATE uint 	cur_loc_index;

enum {CHAR=0, COLOR};

void cons_init(void){
  cur_loc_index = 0;

  cur_loc = (ushort*)VGA_MEM_BASE;
}

void cons_putchar(char c){
  int i;

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

  /* check if we need to move memory up */
  if (cur_loc_index > CONS_MAX_COL * CONS_MAX_ROW){
    /* scroll console up by one line */
    for (i = 0; i < CONS_SIZE - CONS_MAX_COL; i++){
      cur_loc[i] = cur_loc[i + CONS_MAX_COL];
    }

    for (i = CONS_SIZE - CONS_MAX_COL; i < CONS_SIZE; i++){
      cur_loc[i] = DEFAULT_COLOR;
      cur_loc[i] <<= 8;
      cur_loc[i] |= 0;
    }

    cur_loc_index = 0;
  }

  /* Set cursor location */
  outb(0x3D4, 0xE); //cursor location high register
  outb(0x3D5, cur_loc_index >> 8);
  outb(0x3D4, 0xF); // cursor location low register
  outb(0x3D5, cur_loc_index);
}
