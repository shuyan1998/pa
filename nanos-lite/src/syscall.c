#include <common.h>
#include <stdint.h>
#include "am.h"
#include "syscall.h"

void sys_write(intptr_t buf, int count) {
  for(int i = 0; i < count; i++) {
    putch(*((char*)(buf) + i));
  }
}

void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1; // GPR1 stores system num

  switch (a[0]) {
    case SYS_exit:c->GPRx = 0;printf("Do syscall exit, GPRx is %d\n", c->GPRx);halt(c->GPRx);break;
    case SYS_yield:yield();break;
    case SYS_write:sys_write(c->GPR3, c->GPR4);break;
    case SYS_brk:c->GPRx = 0;break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
}
