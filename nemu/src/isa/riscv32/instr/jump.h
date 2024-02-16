#include "cpu/decode.h"
#include "cpu/exec.h"
#include "rtl/rtl.h"
#include "common.h"

def_EHelper(jal) {
  rtl_addi(s, ddest, &s->pc, 4);
  rtl_j(s, s->pc + (word_t)(id_src1->simm));
  // printf("The address is %x\n", s->pc + (word_t)(id_src1->simm));
  // printf("Current pc is %x\n", s->pc);
  // printf("current simm is %d\n", id_src1->simm);
  

  //rtl_j(s, (word_t)((sword_t)s->pc + (id_src1->simm)));
  //printf("The address is %x\n", (word_t)((sword_t)s->pc + (id_src1->simm)));
}

def_EHelper(jalr) {
  rtl_addi(s, ddest, &s->pc, 4);
  rtl_j(s, (*dsrc1)+(word_t)(id_src2->imm));
  // printf("The address is %x\n", (*dsrc1)+(word_t)(id_src2->imm));
}
