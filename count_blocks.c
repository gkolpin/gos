#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

typedef char* string;

int main(int argc, char **argv){
  string filename = argv[1];
  struct stat s_struct;
  off_t carry = 0;
  int block_size = atoi(argv[2]);

  stat(filename, &s_struct);

  carry = s_struct.st_size % block_size;

  if (carry){
    printf("%d", (s_struct.st_size / block_size) + 1);
  } else {
    printf("%d", s_struct.st_size / block_size);
  }

  return 0;
}
