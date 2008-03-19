#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <fcntl.h>

typedef char* string;

#define PATCH_SEQUENCE 0xEFBEADDE //dead beef
#define BOOT_SIZE 512

int main(int argc, char **argv){
  
  string boot_prog = argv[1];
  unsigned int num_sectors = atoi(argv[2]);
  char buf[BOOT_SIZE];
  unsigned int cur_seq, i;
  unsigned char patch[4] = 
    {
     0x10, /* size of DAP */
     0x00, 
     /* fill this one with number
	of sectors to bring in to mem */
     (unsigned char)num_sectors,
     0x00,
    };

  int fd = open(boot_prog, O_RDWR);

  if (num_sectors > 127){
    printf("ERROR: kernel is too large!!!\n");
    return -1;
  }

  read(fd, buf, BOOT_SIZE);

  for (i = 0; i < BOOT_SIZE - sizeof(unsigned int); i++){
    cur_seq = *(unsigned int*)&buf[i];
    if (cur_seq == PATCH_SEQUENCE){
      break;
    }
  }

  //at this point i is the offset in bytes 
  // in the file to dead beef

  lseek(fd, i, SEEK_SET);
  if (write(fd, (const void*)patch, sizeof(unsigned char) * 4) < 0){
    printf("ERROR!!\n");
  }

  close(fd);

  return 0;
}
