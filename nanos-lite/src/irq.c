#include "am.h"
#include <common.h>
#include "debug.h"
#include "proc.h"
#include "syscall.h"

static Context* do_event(Event e, Context* c) {
  switch (e.event) {
    case EVENT_YIELD:
    case EVENT_IRQ_TIMER:
      Log("Handle yield and timer interupt event, event = %d", e.event);
      return schedule(c);
    case EVENT_SYSCALL:
      do_syscall(c);break;
    case EVENT_ERROR:
      panic("Event error %d", e.event);
    default: panic("Unhandled event ID = %d", e.event);
  }

  return c;
}

void init_irq(void) {
  //Log("Initializing interrupt/exception handler...");
  cte_init(do_event);
}
