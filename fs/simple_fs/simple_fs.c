#include "simple_fs.h"
#include "at_disk_driver.h"
#include "gos.h"

/****** FS INDEX LAYOUT ************

	QUADWORD 1: Number of files
	QUADWORD 2N + 1: absolute sector number of start of file
	QUADWORD 2N + 2: length of file in sectors
*/

PRIVATE uint32 get_file_blocks(uint32 file_id);
PRIVATE uint32 get_file_block_offset(uint32 file_id);

/* fs start is at block 1000 */
#define FS_START 1000

uint32 get_num_files(){
  byte_t buf[HD_SECTOR_SIZE];

  /* fs index is the first block of the file system */
  disk_read_sector(FS_START, buf);

  /* first quad word contains the number of files */
  return ((uint32*)buf)[0];
}

uint32 get_file_size(uint32 file_id){
  return get_file_blocks(file_id) * HD_SECTOR_SIZE;
}

void load_file(uint32 file_id, byte_t* buf){
  uint32 file_blocks = get_file_blocks(file_id);
  uint32 i;
  uint32 offset = get_file_block_offset(file_id);

  for (i = 0; i < file_blocks; i++){
    disk_read_sector(offset + i, &buf[i * HD_SECTOR_SIZE]);
  }
}

PRIVATE uint32 get_file_blocks(uint32 file_id){
  byte_t buf[HD_SECTOR_SIZE];
  
  /* read fs index */
  disk_read_sector(FS_START, buf);
  
  /* refer to fs index layout above for rationale of following */
  return ((uint32*)buf)[file_id * 2 + 2];
}

PRIVATE uint32 get_file_block_offset(uint32 file_id){
  byte_t buf[HD_SECTOR_SIZE];
  
  /* read fs index */
  disk_read_sector(FS_START, buf);
  
  /* refer to fs index layout above for rationale of following */
  return ((uint32*)buf)[file_id * 2 + 1];
}
