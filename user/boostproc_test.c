// user/boostproc_test.c

#include "kernel/types.h"
#include "user/user.h"

int
main(void)
{
  printf("Boosting all processes...\n");
  boostproc();
  exit(0);
}
