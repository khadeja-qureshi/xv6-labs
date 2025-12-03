// user/getprocinfo.c
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  if (argc != 2) {
    printf("usage: getprocinfo pid\n");
    exit(1);
  }

  int pid = atoi(argv[1]);
  struct procinfo pi;

  if (getprocinfo(pid, &pi) < 0) {
    printf("getprocinfo: failed for pid %d\n", pid);
    exit(1);
  }

  printf("pid=%d state=%d priority=%d\n", pi.pid, pi.state, pi.priority);
  for (int i = 0; i < NQUEUE; i++) {
    // NOTE: xv6 printf doesn’t support %l for uint64
    printf("  queue %d ticks=%d\n", i, (int)pi.total_ticks_per_queue[i]);
  }

  exit(0);
}
