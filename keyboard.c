#include "keyboard.h"
#include "portio.h"
#include "types.h"
#include "gos.h"

/* Driver for the 8042 keyboard controller */

/* keyboard status input waiting bit */
#define KBS_IBF		0x1
#define KB_ACK		0xFA
#define KB_SSC		0xF0
#define KB_SCS_2	0x02
#define KB_SIBF_DISABLE	0xFE
#define KB_SOBF		0x1

#define KBSTATP		0x64
#define KBBUFPORT	0x60

#define NO		0

PRIVATE int getkbdata();

PRIVATE uint8 kb2ascii[256] =
  {
    NO,   0x1B, '1',  '2',  '3',  '4',  '5',  '6',  // 0x00
    '7',  '8',  '9',  '0',  '-',  '=',  '\b', '\t',
    'q',  'w',  'e',  'r',  't',  'y',  'u',  'i',  // 0x10
    'o',  'p',  '[',  ']',  '\n', NO,   'a',  's',
    'd',  'f',  'g',  'h',  'j',  'k',  'l',  ';',  // 0x20
    '\'', '`',  NO,   '\\', 'z',  'x',  'c',  'v',
    'b',  'n',  'm',  ',',  '.',  '/',  NO,   '*',  // 0x30
    NO,   ' ',  NO,   NO,   NO,   NO,   NO,   NO,
    NO,   NO,   NO,   NO,   NO,   NO,   NO,   '7',  // 0x40
    '8',  '9',  '-',  '4',  '5',  '6',  '+',  '1',
    '2',  '3',  '0',  '.',  NO,   NO,   NO,   NO   // 0x50
  };

void kbd_init(){
}

/* get ascii character of key pressed */
char getchar(){
  int cdata;
  while ((cdata = getkbdata()) == -1);
  return cdata;
}

/* return -1 if no valid data to be returned */
PRIVATE int getkbdata(){
  uint8 data;

  while ((inb(KBSTATP) & KBS_IBF) == 0);

  data = inb(KBBUFPORT);

  /* have we released a key? */
  if (data & 0x80){
    return -1;
  }

  return kb2ascii[data];   
}
