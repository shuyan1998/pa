#include "cpu/decode.h"
#include "cpu/exec.h"
#include "rtl/rtl.h"
#include "common.h"

#define def_control_signed(name, condition) \
    def_EHelper(name) { \
        if((sword_t)*dsrc1 condition (sword_t)*dsrc2){ \
            rtl_j(s, s->pc + id_dest->imm); \
        } \
    }

#define def_control_unsighed(name, condition) \
    def_EHelper(name ## u) { \
        if(*dsrc1 condition *dsrc2){ \
            rtl_j(s, s->pc + id_dest->imm); \
        } \
    }

#define def_control(name, condition) \
    def_control_signed(name, condition); \
    def_control_unsighed(name, condition);

def_control_signed(beq, ==);
def_control_signed(bne, !=);
def_control(blt, <);
def_control(bge, >=);