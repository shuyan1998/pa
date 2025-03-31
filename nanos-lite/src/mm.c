#include "am.h"
#include "klib-macros.h"
#include "proc.h"
#include <memory.h>
#include <stdint.h>

static void *pf = NULL;

void* new_page(size_t nr_page) {
  pf = (void *)ROUNDDOWN(pf + nr_page * PGSIZE, PGSIZE);
  return pf;
}

#ifdef HAS_VME
static void* pg_alloc(int n) {
  size_t nr_page = ROUNDDOWN(n,PGSIZE) / PGSIZE;
  if (n <= 0) {
    panic("Phsyical page alloc invalid!");
  }
  void* pg_ptr = new_page(nr_page);
  for(int i = 0; i < nr_page; i++){
    for(int j = 0; j < PGSIZE; j++){
      *(char*)((uintptr_t)pg_ptr + i * PGSIZE + j) = 0;
    }
  }
  return pg_ptr;
}
#endif

void free_page(void *p) {
  panic("not implement yet");
}

/* The brk() system call handler. */
int mm_brk(uintptr_t brk) {
  extern PCB *current;
  extern char _end;

  if(current->max_brk == 0){
    current->max_brk = (uintptr_t)&_end;
  }
  void *va = NULL, *pa = NULL;
  void* begin = (void*)current->max_brk;
  void* end = (void*)brk;
  int pgsz = ((uintptr_t)end - (uintptr_t)begin  + PGSIZE - 1)/ PGSIZE;
  if(brk >= current->max_brk) {
    for(int i = 0; i < pgsz; i++){
      map(&current->as, va + i*PGSIZE, pa, 0);
    }
    current->max_brk = brk;
  }
  return 0;
}

void init_mm() {
  pf = (void *)ROUNDUP(heap.start, PGSIZE);
  Log("free physical pages starting from %p", pf);

#ifdef HAS_VME
  printf("9999999999999999999\n");
  vme_init(pg_alloc, free_page);
#endif
}
