#include "cpu/decode.h"
#include "cpu/exec.h"
#include "rtl/rtl.h"
#include <stdio.h>
def_EHelper(lw) {
  //printf("lw +++++= dest %x, dsrc1 %x, offset is %x, pc is %x\n", *ddest, *dsrc1, id_src2->imm, cpu.pc);
  rtl_lms(s, ddest, dsrc1, id_src2->imm, 4);
}

def_EHelper(lh) {
  rtl_lms(s, ddest, dsrc1, id_src2->imm, 2);
}

def_EHelper(lb) {
  rtl_lms(s, ddest, dsrc1, id_src2->imm, 1);
}

def_EHelper(lbu) {
  rtl_lm(s, ddest, dsrc1, id_src2->imm, 1);
}

def_EHelper(lhu){
  rtl_lm(s, ddest, dsrc1, id_src2->imm, 2);
}

def_EHelper(sw) {
  //printf("sw +++++= dest %x, dsrc1 %x, offset is %x, pc is %x\n", *ddest, *dsrc1, id_src2->imm, cpu.pc);
  rtl_sm(s, ddest, dsrc1, id_src2->imm, 4);
}

def_EHelper(sb) {
  rtl_sm(s, ddest, dsrc1, id_src2->imm, 1);
}

def_EHelper(sh){
  rtl_sm(s, ddest, dsrc1, id_src2->imm, 2);
}
