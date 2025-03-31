#include <isa.h>

#define MSTATUS_MPIE_MASK 0x00000080
#define MSTATUS_MIE_MASK 0x00000008
word_t isa_raise_intr(word_t NO, vaddr_t epc) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * Then return the address of the interrupt/exception vector.
   */

  // epc += 4;
  // printf("epc: %x\n", epc);

  // IFDEF(CONFIG_ETRACE, printf("Exception NO: %d\n", NO));
  cpu.csr.mcause = NO;
  cpu.csr.mepc = epc;


  if(NO == 0x80000007){
    // set mstatus.MIE to mstatus.MPIE
    if(cpu.csr.mstatus & MSTATUS_MIE_MASK) {
      cpu.csr.mstatus |= MSTATUS_MPIE_MASK;
    }else {
      cpu.csr.mstatus &= MSTATUS_MPIE_MASK;
    }
    // set mstatus.MIE to 0
    cpu.csr.mstatus &= ~MSTATUS_MIE_MASK;
  }
  printf("Address is %x\n", cpu.csr.mtvec);
  
  return cpu.csr.mtvec;
}

#define IRQ_TIMER 0x80000007  // for riscv32

word_t isa_query_intr() {
  if ( cpu.INTR == true ) {
    cpu.INTR = false;
    return IRQ_TIMER;
  }
  return INTR_EMPTY;
}
