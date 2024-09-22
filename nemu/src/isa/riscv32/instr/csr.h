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

def_EHelper(mret) {
    //cpu.pc = cpu.csr.mepc + 4;
    rtl_j(s, cpu.csr.mepc + 4);
}