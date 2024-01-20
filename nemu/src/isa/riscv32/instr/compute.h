#include "cpu/decode.h"
#include "cpu/exec.h"
#include "rtl/rtl.h"

def_EHelper(lui) {
  rtl_li(s, ddest, id_src1->imm);
}

def_EHelper(addi){
  rtl_addi(s, ddest, dsrc1, id_src2->imm);
}

def_EHelper(auipc){
  rtl_auipc(s, ddest, id_src1->imm);
}
