#include <cpu/decode.h>
#include "../local-include/rtl.h"

// //f(slti) f(sltiu) f(add) f(sub) f(slt) f(sltu)

#define INSTR_LIST(f) f(lui) f(auipc) \
                        f(addi) f(add) f(sub) f(sltiu) f(xori) \
                        f(jal) f(jalr) \
                        f(beq) f(bne) f(bgeu) \
                        f(andi) \
                        f(lw) f(lbu) f(sw) f(sb) \
                        f(inv) f(nemu_trap)

def_all_EXEC_ID();
