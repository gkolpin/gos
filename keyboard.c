#include "kprintf.h"
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
#define KB_GETCMD	0x20
#define KB_SETCMD	0x60

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

PRIVATE uint8 kb2ascii_shift[256] = 
  {
    NO,   033,  '!',  '@',  '#',  '$',  '%',  '^',  // 0x00
    '&',  '*',  '(',  ')',  '_',  '+',  '\b', '\t',
    'Q',  'W',  'E',  'R',  'T',  'Y',  'U',  'I',  // 0x10
    'O',  'P',  '{',  '}',  '\n', NO,   'A',  'S',
    'D',  'F',  'G',  'H',  'J',  'K',  'L',  ':',  // 0x20
    '"',  '~',  NO,   '|',  'Z',  'X',  'C',  'V',
    'B',  'N',  'M',  '<',  '>',  '?',  NO,   '*',  // 0x30
    NO,   ' ',  NO,   NO,   NO,   NO,   NO,   NO,
    NO,   NO,   NO,   NO,   NO,   NO,   NO,   '7',  // 0x40
    '8',  '9',  '-',  '4',  '5',  '6',  '+',  '1',
    '2',  '3',  '0',  '.',  NO,   NO,   NO,   NO,   // 0x50
  };

void kbd_init(){
  uint8 cmd_byte;

  while (inb(KBSTATP) & 2); 	/* wait until input buffer empty */
  outb(KBSTATP, KB_GETCMD);	/* send get command byte command */
  cmd_byte = inb(KBBUFPORT);
  cmd_byte |= 1;		/* set command byte so that keyboard will generate irq 1 */
  outb(KBSTATP, KB_SETCMD);	/* send set command */
  outb(KBBUFPORT, cmd_byte);	/* send command byte */
}

/* get ascii character of key pressed */
char getchar(){
  int cdata;
  while ((cdata = getkbdata()) == -1);
  return cdata;
}

PRIVATE bool is_shifted = 0;

/* return -1 if no valid data to be returned */
PRIVATE int getkbdata(){
  uint8 data;

  while ((inb(KBSTATP) & KBS_IBF) == 0);

  data = inb(KBBUFPORT);

  /* have we released a key? */
  if (data & 0x80){
    if (data == (0x2A | 0x80) ||
	data == (0x36 | 0x80)){
      /* left shift released */
      is_shifted = 0;
    }

    return -1;
  }

  if (data == 0x2A ||
      data == 0x36){
    is_shifted = TRUE;
    return -1;
  }

  if (is_shifted){
    return kb2ascii_shift[data];
  } else {
    return kb2ascii[data];
  }
}
