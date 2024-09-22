#include <common.h>
#include "am.h"
#include "syscall.h"
void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1; // GPR1 stores system num

  switch (a[0]) {
    case SYS_exit:printf("Do syscall exit, GPRx is %d\n", c->GPRx);halt(c->GPRx);break;
    case SYS_yield:yield();break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
}
