// user/yield_test.c
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  int pid = fork();
  if (pid < 0) {
    printf("yield_test: fork failed\n");
    exit(1);
  }

  if (pid == 0) {
    // child: does small chunks of "work" in a loop and queries getprocinfo
    int self = getpid();
    struct procinfo pi;

    for (int i = 0; i < 50; i++) {
      // pretend to do some work
      for (volatile int j = 0; j < 1000000; j++)
        ;

      if (getprocinfo(self, &pi) == 0) {
        int total_ticks = 0;
        for (int q = 0; q < NQUEUE; q++)
          total_ticks += (int)pi.total_ticks_per_queue[q];

        printf("yield_test child: iter=%d priority=%d total_ticks=%d\n",
               i, pi.priority, total_ticks);
      }
    }

    exit(0);
  } else {
    // parent: wait for child
    wait(0);
  }

  exit(0);
}
