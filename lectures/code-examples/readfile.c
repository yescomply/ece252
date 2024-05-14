#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

void readfile(int);

int main(int argc, char** argv) {
  if (argc != 2) {
    printf("Usage: %s <filename>\n", argv[0]);
    return -1;
  }

  int fd = open(argv[1], O_RDONLY);
  if (fd == -1) {
    printf("Unable to open the file! %s is invalid name?\n", argv[1]);
    return -1;
  }

  readfile(fd);
  close(fd);

  return 0;
}

void readfile(int fd) {
  int buf_size = 256;
  char* buffer = malloc(buf_size);
  while(1) {
    memset(buffer, 0, buf_size);
    int bytes_read = read(fd, buffer, buf_size - 1);
    if (bytes_read == 0) {
      break;
    }
    printf("%s", buffer);
  }
  printf("\nEnd of File.\n");
  free(buffer);
}