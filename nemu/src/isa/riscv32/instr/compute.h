#include "common.h"
#include "cpu/decode.h"
#include "cpu/exec.h"
#include "rtl/rtl.h"

#define def_compute_reg(name, rtl_func) def_EHelper(name) { \
  rtl_ ## rtl_func(s, ddest, dsrc1, dsrc2); \
}

#define def_compute_imm(name, rtl_func) def_EHelper(name ## i) { \
  rtl_ ## rtl_func ## i(s, ddest, dsrc1, id_src2->imm); \
}

#define def_compute_reg_imm(name, rtl_func) \
  def_compute_reg(name, rtl_func); \
  def_compute_imm(name, rtl_func);


// compute
def_compute_reg_imm(add, add)
def_compute_reg(sub, sub)
def_compute_reg_imm(xor, xor)
def_compute_reg_imm(or, or)
def_compute_reg_imm(and, and)
def_compute_reg_imm(sll, sll)
def_compute_reg_imm(srl, srl)
def_compute_reg_imm(sra, sra)


// compute slt
def_EHelper(slt) {
  if((sword_t)*dsrc1 < (sword_t)*dsrc2) {
    *ddest = 1;
  }
  else{
    *ddest = 0;
  }
}

def_EHelper(slti) {
  if((sword_t)*dsrc1 < (sword_t)(id_src2->imm)){
    *ddest = 1;
  }
  else{
    *ddest = 0;
  }
}
def_EHelper(sltu){
  if(*dsrc1 < *dsrc2){
    *ddest = 1;
  }
  else{
    *ddest =0;
  }
}

def_EHelper(sltiu) {
  if(*dsrc1 < (id_src2->imm)){
    *ddest = 1;
  }
  else{
    *ddest = 0;
  }
}

// mul/div
def_compute_reg(mul, mulu_lo)
def_compute_reg(mulhu, mulu_hi)
def_compute_reg(div, divs_q)
def_compute_reg(divu, divu_q)
def_compute_reg(rem, divs_r)
def_compute_reg(remu, divu_r)

def_EHelper(lui) {
  rtl_li(s, ddest, id_src1->imm);
}

def_EHelper(auipc){
  rtl_auipc(s, ddest, id_src1->imm);
}