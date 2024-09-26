#include <common.h>
#include <stdint.h>
#include "am.h"
#include "amdev.h"
#include "klib-macros.h"
#include "syscall.h"
#include <fs.h>
#include <stdio.h>

#define NR_FILES 24
void sys_write(intptr_t buf, int count) {
  for(int i = 0; i < count; i++) {
    putch(*((char*)(buf) + i));
  }
}

int sys_gettimeofday(struct timeval *tv, struct timezone *tz) {
  uint32_t usec = io_read(AM_TIMER_UPTIME).us;
  printf("usec is %d\n", usec);
  tv->tv_sec = usec / 1000000;
  tv->tv_usec = usec - usec / 1000000 * 1000000;
  
  return 0;
}

void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1; // GPR1 stores system num

  switch (a[0]) {
    case SYS_exit:printf("Do syscall exit, GPRx is %d\n", c->GPRx);halt(c->GPRx);break;
    case SYS_yield:yield();break;
    case SYS_write:
      c->GPRx = fs_write(c->GPR2, (char*)c->GPR3, c->GPR4);break;
    case SYS_brk:c->GPRx = 0;break;
    case SYS_open:
      c->GPRx = fs_open((char*)c->GPR2, c->GPR3, c->GPR4);break;
    case SYS_close:
      c->GPRx = fs_close(c->GPR2);break;
    case SYS_read:
      c->GPRx = fs_read(c->GPR2, (char*)c->GPR3, c->GPR4);break;
    case SYS_lseek:
      c->GPRx = fs_lseek(c->GPR2, c->GPR3, c->GPR4);break;
    case SYS_gettimeofday:
      c->GPRx = sys_gettimeofday((void*)c->GPR2, (void*)c->GPR3);break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
}
