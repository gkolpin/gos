/* This program will write the index of files on the disk
   image. The index will be at sector 200. The index will be quadword pairs
   that give the base and the length of the program(s), respectively. Before
   the pairs start, however, there will be a single quadword that gives the 
   number of pairs in the index. */


#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <fcntl.h>
#include <sys/stat.h>

#define FILENAME 1
#define SECTOR_SIZE 512
#define ENTRIES_LOC 0
#define SECTOR_START 1001

/* arguments to this will be names of binary program files */
int main(int argc, char **argv){
  char *filename = argv[FILENAME];
  int fd = open(filename, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
  /* buffer to be written - will be the size of a sector */
  int buf[SECTOR_SIZE / sizeof(int)];
  
  int argv_i, buf_loc;
  int cur_sect = SECTOR_START;
  int num_entries;
  for (argv_i = 2, buf_loc = 1, num_entries = 0; 
       argv_i < argc && buf_loc < SECTOR_SIZE; 
       argv_i++, buf_loc += 2, num_entries++){
    buf[buf_loc] = cur_sect;
    buf[buf_loc + 1] = atoi(argv[argv_i]);
    cur_sect += buf[buf_loc + 1];
  }

  buf[ENTRIES_LOC] = num_entries;

  if (write(fd, buf, SECTOR_SIZE) == -1){
    return -1;
  }
  
  return 0;
}
