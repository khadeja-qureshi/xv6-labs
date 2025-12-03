// kernel/procinfo.h
#pragma once
#include "types.h"
#include "param.h"   // for NQUEUE

struct procinfo {
  int pid;
  int state;
  int priority;
  uint64 total_ticks_per_queue[NQUEUE];
};
