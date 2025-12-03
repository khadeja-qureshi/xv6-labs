#include "kernel/types.h"
#include "kernel/param.h"
#include "user/user.h"   // struct procinfo, fork, sleep, printf, getprocinfo, etc.

int
main(void)
{
  int pid = fork();
  if (pid < 0) {
    printf("io_demo: fork failed\n");
    exit(1);
  }

  if (pid == 0) {
    // Child: I/O-ish process (does a bit of work, then SLEEPS).
    for (int i = 0; i < 50; i++) {
      struct procinfo pi;
      if (getprocinfo(getpid(), &pi) == 0) {
        printf("io_child: iter=%d priority=%d total_ticks=%ld\n",
               i,
               pi.priority,
               (long)pi.total_ticks_per_queue[pi.priority]);
      }
      // Sleep for 5 ticks -> blocks, resets slice (good I/O-like behavior)
      sleep(5);
    }
    exit(0);
  } else {
    // Parent: CPU-bound process (busy loop, no sleep)
    volatile int x = 0;
    for (long i = 0; i < 500000000L; i++) {
      x += i;
      // Occasionally print our own scheduling info
      if (i % 50000000L == 0) {
        struct procinfo pi;
        if (getprocinfo(getpid(), &pi) == 0) {
          printf("cpu_parent: iter=%ld priority=%d total_ticks=%ld\n",
                 i / 50000000L,
                 pi.priority,
                 (long)pi.total_ticks_per_queue[pi.priority]);
        }
      }
    }
    wait(0);
    exit(0);
  }
}
