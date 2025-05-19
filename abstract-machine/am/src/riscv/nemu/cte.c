#include "arch/riscv32-nemu.h"
#include <am.h>
#include <riscv/riscv.h>
#include <klib.h>
#include <stdint.h>

static Context* (*user_handler)(Event, Context*) = NULL;

Context* __am_irq_handle(Context *c) {
  __am_get_cur_as(c);
  printf("333 pdir : %x\n", c->pdir);
  if (user_handler) {
    Event ev = {0};
    // printf("mcause is %d\n", c->mcause);
    switch (c->mcause) {
      case -1: 
        ev.event = EVENT_YIELD; break;
      case 0:
      case 1:
      case 2:
      case 3:
      case 4:
      case 5:
      case 6:
      case 7:
      case 8:
      case 9:
      case 10:
      case 11:
      case 12:
      case 13:
      case 14:
      case 15:
      case 16:
      case 17:
      case 18:
      case 19:
        ev.event = EVENT_SYSCALL; break;
      case 0x80000007:
        ev.event = EVENT_IRQ_TIMER; break;
      default: ev.event = EVENT_ERROR; break;
    }

    c = user_handler(ev, c);
    printf("222 pdir : %x\n", c->pdir);
    printf(" mcause is %d\n", c->mcause);
    assert(c != NULL);
  }

  printf("111 pdir : %x\n", c->pdir);
  __am_switch(c);
  return c;
}

extern void __am_asm_trap(void);

bool cte_init(Context*(*handler)(Event, Context*)) {
  // initialize exception entry
  asm volatile("csrw mtvec, %0" : : "r"(__am_asm_trap));

  // register event handler
  user_handler = handler;

  return true;
}
#define MSTATUS_MPIE_MASK 0x00000080
#define MSTATUS_MIE_MASK 0x00000008
Context *kcontext(Area kstack, void (*entry)(void *), void *arg) {
  Context *c = (Context*)kstack.end - 1;
  c->mepc = (uintptr_t)entry;
  c->pdir = NULL;
  c->np = 1;
  c->gpr[2] = (uintptr_t)kstack.end - 36*4;
  //printf("address kcontext %x\n", &c->pdir);
  c->GPR2 = (uintptr_t)arg;
  c->mstatus |= MSTATUS_MPIE_MASK;
  //printf("kernel thread pdir is %x\n", c->pdir);
  //printf("上下文地址 %x, entry地址 %x\n", c, c->mepc);
  //printf("aaa pdir is %x\n", ((Context*)0x8244df71)->pdir);

  return c;
}

void yield() {
  asm volatile("li a7, -1; ecall");
}

bool ienabled() {
  return false;
}

void iset(bool enable) {
}
