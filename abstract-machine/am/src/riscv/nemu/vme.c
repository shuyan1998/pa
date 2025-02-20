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
  return satp << 12;
}

bool vme_init(void* (*pgalloc_f)(int), void (*pgfree_f)(void*)) {
  pgalloc_usr = pgalloc_f;
  pgfree_usr = pgfree_f;

  kas.ptr = pgalloc_f(PGSIZE);

  int i;
  for (i = 0; i < LENGTH(segments); i ++) {
    void *va = segments[i].start;
    for (; va < segments[i].end; va += PGSIZE) {
      map(&kas, va, va, 0);
    }
  }

  set_satp(kas.ptr);
  vme_enable = 1;

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
}

void __am_switch(Context *c) {
  if (vme_enable && c->pdir != NULL) {
    set_satp(c->pdir);
  }
}

void map(AddrSpace *as, void *va, void *pa, int prot) {
  // get page number of first and second page table
  uint32_t vpn_0 = ((uintptr_t)va >> 12) & 0x3ff;
  uint32_t vpn_1 = ((uintptr_t)va >> 22) & 0x3ff;

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

Context *ucontext(AddrSpace *as, Area kstack, void *entry) {
  Context *uc = (Context*)kstack.end - 1;
  uc->mepc = (uintptr_t)entry;

  return uc;
}
