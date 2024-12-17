#include "am.h"
#include <proc.h>
#include <stdint.h>

#define MAX_NR_PROC 4

static PCB pcb[MAX_NR_PROC] __attribute__((used)) = {};
static PCB pcb_boot = {};
PCB *current = NULL;

void switch_boot_pcb() {
  current = &pcb_boot;
}

void hello_fun(void *arg) {
  int j = 1;
  while (1) {
    Log("Hello World from Nanos-lite with arg '%s' for the %dth time!", (char*)arg, j);
    j ++;
    yield();
  }
}

void context_kload(PCB *pcb, void (*entry)(void *), void *arg){
  Context *c = kcontext((Area){pcb->stack, pcb->stack + 1}, entry, arg);
  pcb->cp = c;
}

void init_proc() {
  context_kload(&pcb[0], hello_fun, "A");
  //context_kload(&pcb[1], hello_fun, "B");
  char* argv[] = {"/bin/exec-test", "print", NULL};
  char* envp[] = {"lalala=test", NULL};
  //context_uload(&pcb[1], "/bin/exec-test", argv, envp);
  context_uload(&pcb[1], "/bin/menu", argv, envp);
  switch_boot_pcb();

  Log("Initializing processes...");

  // load program here
  // naive_uload(NULL, "/bin/exec-test");

}

Context* schedule(Context *prev) {
  // save the context pointer
  current->cp = prev;

  // always select pcb[0] as the new process
  current = (current == &pcb[0] ? &pcb[1] : &pcb[0]);

  // then return the new context
  return current->cp;
}

int sys_execve(const char *fname, char * const argv[], char *const envp[]) {
  context_uload(current, fname, argv, envp);
  switch_boot_pcb();
  yield();

  return -1;
}
