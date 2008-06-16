#ifndef _SIMPLE_FS_H
#define _SIMPLE_FS_H

#include "types.h"

uint32 get_num_files();
/* file size in bytes */
uint32 get_file_size(uint32 file_id);
void load_file(uint32 file_id, byte_t* buf);

#endif
