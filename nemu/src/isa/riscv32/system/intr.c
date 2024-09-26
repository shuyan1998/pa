#include <isa.h>

word_t isa_raise_intr(word_t NO, vaddr_t epc) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * Then return the address of the interrupt/exception vector.
   */

  //epc += 4;
  //printf("epc: %x\n", epc);

  // IFDEF(CONFIG_ETRACE, printf("Exception NO: %d\n", NO));
  cpu.csr.mcause = NO;
  cpu.csr.mepc = epc;
  return cpu.csr.mtvec;
}

word_t isa_query_intr() {
  return INTR_EMPTY;
}
