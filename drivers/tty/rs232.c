#include "gos.h"
#include "rs232.h"
#include "types.h"

#define IO_PORT 0x3F8

void serial_init(void){
  uint tmp;
  outb(IO_PORT + 3, 3);
  /* set baud divisor */
  tmp = inb(IO_PORT + 3);
  tmp |= 0x80;
  outb(IO_PORT + 3, tmp);
  outb(IO_PORT, 2);
  outb(IO_PORT + 1, 0);

  outb(IO_PORT + 3, tmp ^ 0x80);
}

void serial_putchar(char c){
  //outb(IO_PORT + 4, 3);
  /*tmp = inb(IO_PORT + 6);
    while (!(tmp & 0x30));*/
  while (!(inb(IO_PORT + 5) & 0x20));
  outb(IO_PORT, c);
}
