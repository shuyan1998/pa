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
  Context *c = kcontext((Area){pcb->stack, pcb->stack + sizeof(pcb->stack)}, entry, arg);
  //printf("jjj pdir is %x\n", ((Context*)0x8244df71)->pdir);
  //c->pdir = NULL;
  //printf("上下文地址 %x, entry地址 %x\n", c, c->mepc);
  pcb->cp = c;

  //printf("c is %x, pcb addr is %x, pdir add is %x\n", c, &pcb->cp, &c->pdir);
  //printf("ggg pdir is %x\n", ((Context*)0x8244df71)->pdir);
}

void init_proc() {
  //context_kload(&pcb[0], hello_fun, "A");
  //printf("ccc pdir is %x\n", ((Context*)0x8244df71)->pdir);
  //context_kload(&pcb[1], hello_fun, "B");
  //printf("ddd pdir is %x\n", ((Context*)0x8244df71)->pdir);
  char* argv[] = {"/bin/dummy", NULL};
  char* envp[] = {"lalala=test", NULL};
  //context_uload(&pcb[1], "/bin/exec-test", argv, envp);
  context_uload(&pcb[0], "/bin/dummy", argv, NULL);
  //context_uload(&pcb[1], "/bin/dummy", argv, NULL);
  //context_uload(&pcb[0], "/bin/pal", argv, NULL);
  switch_boot_pcb();

  Log("Initializing processes...");
  //printf("bbb pdir is %x\n", ((Context*)0x8244df71)->pdir);

  // load program here
  // naive_uload(NULL, "/bin/exec-test");

}

Context* schedule(Context *prev) {
  // save the context pointer
  current->cp = prev;

  // always select pcb[0] as the new process
  // current = (current == &pcb[0] ? &pcb[1] : &pcb[0]);
  current = &pcb[0];

  // then return the new context
  return current->cp;
}

int sys_execve(const char *fname, char * const argv[], char *const envp[]) {
  context_uload(current, fname, argv, envp);
  switch_boot_pcb();
  yield();

  return -1;
}
