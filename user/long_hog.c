// user/long_hog.c
#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/param.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  int pid = getpid();
  struct procinfo pi;

  printf("long_hog: starting, pid=%d\n", pid);

  // very long busy loop so the process stays alive
  for (long i = 0; i < 1000000000L; i++) {
    // every 100,000,000 iterations, print scheduling info
    if (i % 100000000L == 0) {
      if (getprocinfo(pid, &pi) == 0) {
        int total_ticks = 0;
        for (int q = 0; q < NQUEUE; q++)
          total_ticks += (int)pi.total_ticks_per_queue[q];

        printf("long_hog: pid=%d priority=%d total_ticks=%d (i=%ld)\n",
               pi.pid, pi.priority, total_ticks, i);
      }
    }
  }

  printf("long_hog: done\n");
  exit(0);
}
