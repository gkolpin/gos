#include "at_disk_driver.h"
#include "portio.h"
#include "types.h"
#include "gos.h"
#include "mm.h"

/* hard disk data port */
#define HD_DATA	0x1F0
/* hard disk command port */
#define HD_COMMAND 0x1F7
/* hard disk status */
#define HD_STATUS 0x1F7
/* controller busy mask */
#define HD_BUSY 0x80
/* controller ready to receive or send data mask */
#define HD_READY 0x8
/* sector count */
#define HD_SECT_CNT 0x1F2
/* sector number */
#define HD_SECT_NO 0x1F3
/* cylinder low */
#define HD_CYL_LOW 0x1F4
/* cylinder high */
#define HD_CYL_HIGH 0x1F5
/* drive and head register */
#define HD_HEAD 0x1F6
/* LBA flags */
#define LBA_FLAGS 0xE0 
/* sector size */
#define HD_SECTOR_SIZE 512

PRIVATE void set_sector_no(uint32);
PRIVATE void wait_for_ready();

void disk_read_sector(uint32 sector, uint8* buf){
  /* wait until disk is ready */
  wait_for_ready();

  /* set one sector to be transferred */
  outb(HD_SECT_CNT, 1);

  set_sector_no(sector);

  ins(HD_DATA, (word_t*)buf, HD_SECTOR_SIZE / 2);
}

PRIVATE void set_sector_no(uint32 sector){
  /* set sector number to read */
  outb(HD_SECT_NO, sector & 0xFF);   /* LBA bits 0 - 7 */
  outb(HD_CYL_LOW, sector & 0xFF00); /* LBA bits 8 - 15 */
  outb(HD_CYL_HIGH, sector & 0xFF0000);	/* LBA bits 16 - 23 */
  outb(HD_CYL_HIGH, (sector & 0xF000000) | LBA_FLAGS); /* bits 25 - 28 with LBA flags */
}

PRIVATE void wait_for_ready(){
  /* wait for controller not busy */
  while ((inb(HD_STATUS) & HD_BUSY) || !(inb(HD_STATUS) & HD_READY));
}
