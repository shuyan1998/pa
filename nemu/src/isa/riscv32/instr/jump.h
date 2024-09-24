#include "cpu/decode.h"
#include "cpu/exec.h"
#include "rtl/rtl.h"
#include "common.h"
#include "parse_elf.h"

def_EHelper(jal) {
  rtl_addi(s, ddest, &s->pc, 4);
  rtl_j(s, s->pc + (word_t)(id_src1->imm));
  //printf("dest is %x, rz is %x, pc is %x, instr is %s\n", ddest, rz, s->pc, s->logbuf);
  if(s->isa.instr.j.rd == 0) {
    //printf("========\n");
    return;
  }
  get_func_name_with_pc(s->pc, s->pc + (word_t)(id_src1->imm), "/home/shuwang/workspace1/pa/nanos-lite/build/ramdisk.img", 0); 
}

def_EHelper(jalr) {
  
  rtl_j(s, (*dsrc1)+(word_t)(id_src2->imm));
  rtl_addi(s, ddest, &s->pc, 4);
  int mode = 0;
  if(s->isa.instr.j.rd == 0) {
    //printf("hello\n");
    mode = 1;
  }
  //printf("mode is %d\n", mode);
  get_func_name_with_pc(s->pc, (*dsrc1)+(word_t)(id_src2->imm), "/home/shuwang/workspace1/pa/nanos-lite/build/ramdisk.img", mode); 
}
