#include "arch/riscv32-nemu.h"
#include <am.h>
#include <riscv/riscv.h>
#include <klib.h>
#include <stdint.h>

static Context* (*user_handler)(Event, Context*) = NULL;

Context* __am_irq_handle(Context *c) {
  __am_get_cur_as(c);
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
        ev.event = EVENT_SYSCALL; break;
      case 5:
        ev.event = EVENT_IRQ_TIMER; break;
      default: ev.event = EVENT_ERROR; break;
    }

    c = user_handler(ev, c);
    assert(c != NULL);
  }

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
  c->GPR2 = (uintptr_t)arg;
  c->mstatus |= MSTATUS_MIE_MASK;

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
