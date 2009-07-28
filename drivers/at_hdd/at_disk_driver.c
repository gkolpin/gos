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
/* device info command */
#define HD_DEV_INFO 0xEC
/* read multiple sectors, with retries command */
#define HD_READ_MULT_SECTORS 0x20
/* disk error mask */
#define HD_ERROR 0x1
/* error register */
#define HD_ERROR_REG 0x1F1

PRIVATE void set_sector_no(uint32);
PRIVATE void hd_wait(uint8 mask, uint8 stop_wait_val);
/*PRIVATE void disk_get_info(uint8* buf);*/

void hd_driver_init(){

}

/*PRIVATE void disk_get_info(uint8* buf){
  hd_wait(HD_BUSY, 0);

  //used to select hd 0
  set_sector_no(0);
  
  hd_wait(HD_BUSY, 0);

  outb(HD_COMMAND, HD_DEV_INFO);

  hd_wait(HD_BUSY, 0);

  ins(HD_DATA, (word_t*)buf, HD_SECTOR_SIZE / 2);
}*/

void disk_read_sector(uint32 sector, uint8* buf){
  /* wait until disk is not busy */
  hd_wait(HD_BUSY, 0);
  /* set disk and sector number to transfer */
  set_sector_no(sector);
  /* set one sector to be transferred */
  outb(HD_SECT_CNT, 1);

  outb(HD_COMMAND, HD_READ_MULT_SECTORS);

  hd_wait(HD_READY, HD_READY);

  ins(HD_DATA, (word_t*)buf, HD_SECTOR_SIZE / 2);
}

PRIVATE void set_sector_no(uint32 sector){
  /* set sector number to read */
  outb(HD_HEAD, ((sector >> 24) & 0xF) | LBA_FLAGS); /* bits 25 - 28 with LBA flags */
  hd_wait(HD_BUSY, 0);
  outb(HD_SECT_NO, sector & 0xFF);   /* LBA bits 0 - 7 */
  outb(HD_CYL_LOW, (sector >> 8) & 0xFF); /* LBA bits 8 - 15 */
  outb(HD_CYL_HIGH, (sector >> 16) & 0xFF);	/* LBA bits 16 - 23 */
}

PRIVATE void hd_wait(uint8 mask, uint8 stop_wait_val){
  int i;
  /* first 4 - 5 reads of status might be erroneous, so 
     do them before we read the status for real. */
  for (i = 0; i < 5; i++){
    inb(HD_STATUS);
  }
  /* wait for controller not equal to stop_wait_val */
  while ((inb(HD_STATUS) & mask) != stop_wait_val);
  //while ((inb(HD_STATUS) & HD_BUSY) || !(inb(HD_STATUS) & HD_READY));
}
