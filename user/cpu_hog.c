// user/cpu_hog.c
#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/param.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
int pid = getpid();
struct procinfo pi;

printf("cpu_hog: starting, pid=%d\n", pid);

// busy loop to hog CPU
for (int i = 0; i < 20000000; i++) {   // 20M instead of 100M to finish faster
 // every 1,000,000 iterations, print scheduling info
 if (i % 1000000 == 0) {
   if (getprocinfo(pid, &pi) == 0) {
     int total_ticks = 0;
     for (int q = 0; q < NQUEUE; q++)
       total_ticks += (int)pi.total_ticks_per_queue[q];

     printf("cpu_hog: pid=%d priority=%d total_ticks=%d\n",
            pi.pid, pi.priority, total_ticks);
   }
 }
}

printf("cpu_hog: done\n");
exit(0);
}
