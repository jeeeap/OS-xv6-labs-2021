#include "types.h"
#include "riscv.h"
#include "param.h"
#include "defs.h"
#include "date.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void)
{
  int n;
  if(argint(0, &n) < 0)
    return -1;
  exit(n);
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
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  if(argaddr(0, &p) < 0)
    return -1;
  return wait(p);
}

uint64
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;


  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}


#ifdef LAB_PGTBL
int
sys_pgaccess(void)
{
    // 添加变量声明
    uint64 startaddr;  // 起始地址
    int npage;         // 页面数量
    uint64 userbuf;    // 用户空间缓冲区地址

    // 从用户空间获取参数
    if(argaddr(0, &startaddr) < 0) {
        return -1;
    }
    if(argint(1, &npage) < 0) {
        return -1;
    }
    if(argaddr(2, &userbuf) < 0) {
        return -1;
    }

    // 参数验证
    if(npage < 0 || npage > 64) {
        return -1;
    }

    struct proc *p = myproc();
    uint64 bitmap = 0;
    
    // 检查每个页面的访问位
    for(int i = 0; i < npage; i++) {
        uint64 addr = startaddr + i * PGSIZE;
        pte_t *pte = walk(p->pagetable, addr, 0);
        
        if(pte && (*pte & PTE_A)) {
            bitmap |= (1 << i);      // 设置对应的位
            *pte &= ~PTE_A;          // 清除访问位
        }
    }
    
    // 将结果复制回用户空间
    if(copyout(p->pagetable, userbuf, (char *)&bitmap, sizeof(bitmap)) < 0) {
        return -1;
    }
    
    return 0;
}
#endif

uint64
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
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
