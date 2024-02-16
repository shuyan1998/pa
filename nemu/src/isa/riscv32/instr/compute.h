#include "common.h"
#include "cpu/decode.h"
#include "cpu/exec.h"
#include "rtl/rtl.h"

def_EHelper(lui) {
  rtl_li(s, ddest, id_src1->imm);
}

def_EHelper(addi){
  rtl_addi(s, ddest, dsrc1, id_src2->imm);
}

def_EHelper(add){
  rtl_add(s, ddest, dsrc1, dsrc2);
}

def_EHelper(sub) {
  rtl_sub(s, ddest, dsrc1, dsrc2);
}

def_EHelper(sltiu) {
  if((sword_t)*dsrc1 < (id_src2->simm)){
    *ddest = 1;
  }
  else{
    *ddest = 0;
  }
}

def_EHelper(auipc){
  rtl_auipc(s, ddest, id_src1->imm);
}

def_EHelper(andi) {
  rtl_andi(s, ddest, dsrc1, id_src2->imm);
}

def_EHelper(xori) {
  rtl_xori(s, ddest, dsrc1, id_src2->imm);
}
