#include "common.h"
#include "cpu/exec.h"
#include "cpu/decode.h"
#include "isa.h"
#include "rtl/rtl.h"

static word_t *csr_id_instr2address(word_t imm) {
    switch(imm) {
        case 0x341: return &(cpu.csr.mepc);
        case 0x342: return &(cpu.csr.mcause);
        case 0x300: return &(cpu.csr.mstatus);
        case 0x305: return &(cpu.csr.mtvec);
        case 0x340: return &(cpu.csr.mscratch);
        case 0x180: return &(cpu.csr.satp);
        default: panic("unknown csr");
    }
}

#define csr(imm) csr_id_instr2address(imm)
def_EHelper(csrrw) {
    word_t *csr = csr(id_src2->imm);
    *ddest = *csr;
    *csr = *dsrc1;
}

def_EHelper(csrrs) {
    word_t *csr = csr(id_src2->imm);
    *ddest = *csr;
    *csr |= *dsrc1;
}

def_EHelper(ecall) {
    bool success =  false;
    word_t trap_no = isa_reg_str2val("$a7", &success);

    if(!success) {
        Assert(0, "unknown gpr register");
    }
    vaddr_t trap_vec = isa_raise_intr(trap_no, cpu.pc);
    rtl_j(s, trap_vec);
}

#define MSTATUS_MPIE_MASK 0x00000080
#define MSTATUS_MIE_MASK 0x00000008
def_EHelper(mret) {
    if(cpu.csr.mstatus & MSTATUS_MPIE_MASK){
        cpu.csr.mstatus |= MSTATUS_MIE_MASK;
    }else {
        cpu.csr.mstatus &= ~MSTATUS_MIE_MASK;
    }
    cpu.csr.mstatus |= MSTATUS_MPIE_MASK;
    //cpu.pc = cpu.csr.mepc + 4;
    //printf("mret pc is %x\n", cpu.csr.mepc + 4);
    printf("=3 mepc is %x\n", cpu.csr.mepc);

    rtl_j(s, cpu.csr.mepc);
}