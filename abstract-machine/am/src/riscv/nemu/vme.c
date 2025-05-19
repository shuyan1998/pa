#include "riscv/riscv.h"
#include <am.h>
#include <nemu.h>
#include <klib.h>
#include <stdint.h>

static AddrSpace kas = {};
static void* (*pgalloc_usr)(int) = NULL;
static void (*pgfree_usr)(void*) = NULL;
static int vme_enable = 0;

static Area segments[] = {      // Kernel memory mappings
  NEMU_PADDR_SPACE
};

#define USER_SPACE RANGE(0x40000000, 0x80000000)

static inline void set_satp(void *pdir) {
  uintptr_t mode = 1ul << (__riscv_xlen - 1);
  asm volatile("csrw satp, %0" : : "r"(mode | ((uintptr_t)pdir >> 12)));
}

static inline uintptr_t get_satp() {
  uintptr_t satp;
  asm volatile("csrr %0, satp" : "=r"(satp));
  printf("satp is %x\n", satp);
  return satp << 12;
}

bool vme_init(void* (*pgalloc_f)(int), void (*pgfree_f)(void*)) {
  printf("4444444444444444\n");
  pgalloc_usr = pgalloc_f;
  pgfree_usr = pgfree_f;

  printf("5555555555555555555\n");
  kas.ptr = pgalloc_f(PGSIZE);

  printf("22222222222\n");
  int i;
  for (i = 0; i < LENGTH(segments); i ++) {
    void *va = segments[i].start;
    for (; va < segments[i].end; va += PGSIZE) {
      map(&kas, va, va, 0);
    }
  }

  set_satp(kas.ptr);
  printf("kas is %x\n", kas.ptr);
  vme_enable = 1;
  printf("3333333333333333333\n");

  return true;
}

void protect(AddrSpace *as) {
  PTE *updir = (PTE*)(pgalloc_usr(PGSIZE));
  as->ptr = updir;
  as->area = USER_SPACE;
  as->pgsize = PGSIZE;
  // map kernel space
  memcpy(updir, kas.ptr, PGSIZE);
}

void unprotect(AddrSpace *as) {
}

void __am_get_cur_as(Context *c) {
  c->pdir = (vme_enable ? (void *)get_satp() : NULL);
  printf("am_get pdir is %x\n", c->pdir);
}

void __am_switch(Context *c) {
  printf("address switch %x\n", &c->pdir);
  if (vme_enable && c->pdir != NULL) {
    printf("hahahahahhahahahah pdir is %x\n", c->pdir);
    set_satp(c->pdir);
  }
}

void map(AddrSpace *as, void *va, void *pa, int prot) {
  // get page number of first and second page table
  uint32_t vpn_0 = ((uintptr_t)va >> 12) & 0x3ff;
  uint32_t vpn_1 = ((uintptr_t)va >> 22) & 0x3ff;

  if(pa == NULL){
    pa = pgalloc_usr(PGSIZE);
  }

  PTE* page_dir_entry = (PTE*)as->ptr + vpn_1;
  // if page dir entry is not exist, create the page table
  if(!(*page_dir_entry & PTE_V)) {
    PTE* page_table_base = (PTE*)pgalloc_usr(PGSIZE);
    *page_dir_entry = ((PTE)page_table_base) | PTE_V;
    PTE* page_table_entry = page_table_base + vpn_0;
    *page_table_entry = ((uintptr_t)pa & 0xfffff000) | PTE_V | PTE_R | PTE_W | PTE_X;
  }
  else {
  // if page dir entry is exist, get address of the page table
  PTE* page_table_base = (PTE*)(*page_dir_entry & 0xfffff000);
  PTE* page_table_entry = page_table_base + vpn_0;
  *page_table_entry = ((uintptr_t)pa & 0xfffff000) | PTE_V | PTE_R | PTE_W | PTE_X;
  }
}

#define MSTATUS_MPIE_MASK 0x00000080
#define MSTATUS_MIE_MASK 0x00000008
Context *ucontext(AddrSpace *as, Area kstack, void *entry) {
  Context *uc = (Context*)kstack.end - 1;
  uc->mepc = (uintptr_t)entry;
  uc->pdir = as->ptr;
  uc->np = 0;
  uc->gpr[2] = (uintptr_t)kstack.end - 36*4;
  uc->mstatus |= MSTATUS_MPIE_MASK;

  return uc;
}
