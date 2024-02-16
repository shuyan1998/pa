#include "cpu/decode.h"
#include "cpu/exec.h"
#include "rtl/rtl.h"
#include "common.h"

def_EHelper(beq) {
    if(*dsrc1 == *dsrc2){
        rtl_j(s, s->pc + (word_t)(id_dest->simm));
    }
}

def_EHelper(bne) {
    if(*dsrc1 != *dsrc2){
        rtl_j(s, s->pc + (word_t)(id_dest->simm));
    }
}

def_EHelper(bgeu) {
    if((sword_t)*dsrc1 >= (sword_t)*dsrc2){
        rtl_j(s, s->pc + (word_t)(id_dest->simm));
    }
}