// user/sixfive.c
#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

#define SEPCHARS " -\r\t\n./,"

int
main(int argc, char *argv[])
{
  if(argc < 2){
    fprintf(2, "Usage: sixfive file...\n");
    exit(1);
  }

  for(int i = 1; i < argc; i++){
    int fd = open(argv[i], O_RDONLY);
    if(fd < 0){
      fprintf(2, "sixfive: cannot open %s\n", argv[i]);
      exit(1);
    }

    char buf[1];
    char numbuf[32];
    int nidx = 0;

    while(read(fd, buf, 1) == 1){
      char c = buf[0];
      if(strchr(SEPCHARS, c) != 0){

        if(nidx > 0){
          numbuf[nidx] = '\0';
          int val = atoi(numbuf);
          if(val % 5 == 0 || val % 6 == 0){
            printf("%d\n", val);
          }
          nidx = 0;
        }
      } else if(c >= '0' && c <= '9'){
        if(nidx < sizeof(numbuf)-1)
          numbuf[nidx++] = c;
      } else {

        if(nidx > 0){
          numbuf[nidx] = '\0';
          int val = atoi(numbuf);
          if(val % 5 == 0 || val % 6 == 0){
            printf("%d\n", val);
          }
          nidx = 0;
        }
      }
    }

    if(nidx > 0){
      numbuf[nidx] = '\0';
      int val = atoi(numbuf);
      if(val % 5 == 0 || val % 6 == 0){
        printf("%d\n", val);
      }
    }

    close(fd);
  }

  exit(0);
}
