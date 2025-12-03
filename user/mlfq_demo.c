#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(void)
{
  int pid1 = fork();
  if (pid1 == 0) {
    // child 1: cpu hog
    exec("cpu_hog", (char*[]){"cpu_hog", 0});
    printf("exec cpu_hog failed\n");
    exit(1);
  }

  int pid2 = fork();
  if (pid2 == 0) {
    // child 2: yielding process
    exec("yield_test", (char*[]){"yield_test", 0});
    printf("exec yield_test failed\n");
    exit(1);
  }

  // parent waits for yield_test; cpu_hog runs forever
  wait(0);
  printf("mlfq_demo: yield_test finished, killing cpu_hog not implemented (just reset)\n");

  // you can just reboot / reset xv6 after observing output
  for (;;)
    ;

  return 0;
}
