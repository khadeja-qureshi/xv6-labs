#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "vm.h"
#include "procinfo.h"

extern struct proc proc[NPROC];

extern void mlfq_boost(void);
extern struct spinlock tickslock;
extern uint ticks;


uint64
sys_boostproc(void)
{
  mlfq_boost();
  return 0;
}


uint64
sys_getprocinfo(void)
{
  int pid;
  uint64 uaddr;   // user pointer to struct procinfo
  struct procinfo info;
  struct proc *p;
  int found = 0;

argint(0, &pid);
  argaddr(1, &uaddr);
  // search process table
  for (p = proc; p < &proc[NPROC]; p++) {
    acquire(&p->lock);
    if (p->pid == pid) {
      info.pid      = p->pid;
      info.state    = p->state;
      info.priority = p->priority;
      for (int i = 0; i < NQUEUE; i++)
        info.total_ticks_per_queue[i] = p->total_ticks_per_queue[i];
      found = 1;
      release(&p->lock);
      break;
    }
    release(&p->lock);
  }

  if (!found)
    return -1;

  // copy result to user space
  struct proc *me = myproc();
  if (copyout(me->pagetable, uaddr, (char *)&info, sizeof(info)) < 0)
    return -1;

  return 0;
}

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  kexit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return kfork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return kwait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int t;
  int n;

  argint(0, &n);
  argint(1, &t);
  addr = myproc()->sz;

  if(t == SBRK_EAGER || n < 0) {
    if(growproc(n) < 0) {
      return -1;
    }
  } else {
    // Lazily allocate memory for this process: increase its memory
    // size but don't allocate memory. If the processes uses the
    // memory, vmfault() will allocate it.
    if(addr + n < addr)
      return -1;
    myproc()->sz += n;
  }
  return addr;
}

uint64
sys_pause(void)
{
  int n;
  uint ticks0;

  argint(0, &n);
  if(n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  backtrace();
  return 0;


}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  argint(0, &n);

  if (n < 0)
    n = 0;

  acquire(&tickslock);
  ticks0 = ticks;
  while (ticks - ticks0 < (uint)n) {
    if (killed(myproc())) {
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);  // kernel sleep(chan, lock)
  }
  release(&tickslock);
  return 0;
}



uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kkill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

uint64
sys_sigalarm(void)
{
  int ticks;
  uint64 handler;
  struct proc *p = myproc();
  argint(0, &ticks);          // no if (<0) check here
  argaddr(1, &handler);
  p->alarm_interval = ticks;
  p->alarm_handler  = handler;
  p->alarm_ticks    = 0;
  // Optional: clear in_alarm when (re)setting
  // p->in_alarm = 0;

  return 0;
}

uint64
sys_sigreturn(void)
{
  struct proc *p = myproc();

  // Save the original a0 value from the saved trapframe
  uint64 old_a0 = p->alarm_tf.a0;

  // Restore *all* registers, including a0, epc, etc.
  *p->trapframe = p->alarm_tf;

  p->in_alarm = 0;      // allow future alarms

  // Make syscall() write old_a0 into trapframe->a0
  return old_a0;
}

