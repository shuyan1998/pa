#include <cpu/decode.h>
#include "../local-include/rtl.h"

#define INSTR_LIST(f) f(lui) f(auipc) \
                        f(addi) f(add) f(sub) f(sltiu) f(xori) f(slli) f(sltu) \
                        f(or) f(xor) f(sra) f(srl) f(srli) f(srai) f(sll) f(and) \
                        f(jal) f(jalr) \
                        f(beq) f(bge) f(bne) f(bgeu) f(blt) f(bltu) \
                        f(andi) \
                        f(lw) f(lbu) f(lhu) f(sw) f(sb) f(sh) f(lb) \
                        f(mul) f(mulhu) f(div) f(divu) f(rem) f(remu) \
                        f(inv) f(nemu_trap)

def_all_EXEC_ID();
