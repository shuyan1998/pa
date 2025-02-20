#include "common.h"
#include <isa.h>
#include <memory/paddr.h>

word_t vaddr2paddr(vaddr_t addr, int len, int type) {
  int flag = isa_mmu_check(addr, len, type);
  paddr_t paddr = addr;

  switch(flag){
    case MMU_DIRECT:
      break;
    case MMU_TRANSLATE:
      paddr = isa_mmu_translate(addr, len, type);
      break;
    case MMU_FAIL:
    default:
      assert(0);
  }

  return paddr;
}
word_t vaddr_ifetch(vaddr_t addr, int len) {
  paddr_t paddr = vaddr2paddr(addr, len, MEM_TYPE_IFETCH);
  return paddr_read(paddr, len);
}

word_t vaddr_read(vaddr_t addr, int len) {
  paddr_t paddr = vaddr2paddr(addr, len, MEM_TYPE_READ);
  return paddr_read(paddr, len);
}

void vaddr_write(vaddr_t addr, int len, word_t data) {
  paddr_t paddr = vaddr2paddr(addr, len, MEM_TYPE_WRITE);
  paddr_write(addr, len, data);
}
