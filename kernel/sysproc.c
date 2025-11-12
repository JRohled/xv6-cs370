#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "vm.h"

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
sys_debug(void) 
{
  int enable;
  argint(0, &enable);
  //Sets the system level debug to the enable parameter
  //enable != 0 -> debug mode on
  if(enable == 0){
    mode = 0; // Run
  }else if(enable == 1){
    mode = 1; // Debug
  }else{
    printf("Error Invalid value for enable");
    return -1;
  }
  return 0;
}

/*
  TO-DO:
  Implement a new scheduler that accpets priority as an arg.
  Same as fork() except the accepted arg is set 
  as the priority in the PCB of the new process(Range: 0 - 19).
  Return error if <0 or >19. 
  Value of num_epoch_slots = 0
  
*/
uint64
sys_priority_fork(void) 
{
  int priority; 
  argint(0, &priority);
  if(priority < 0) {
    printf("\nPriority is out of range: Lower than 0");
    return -1;
  }else if(priority > 19) {
    printf("\nPriority is out of range: Higher than 19");
    return -1;
  }

  return priority_fork(priority);
} 
