#include "sdb.h"
#include "utils.h"
#include <stdlib.h>

#define NR_BP 32

typedef struct breakpoint{
    int NO;
    struct breakpoint *next;
    char symbol[20];
}BP;

static BP bp_pool[NR_BP] = {};
static BP *head = NULL, *free_ = NULL;

void init_bp_pool(){
    for(int i=0;i<NR_BP;i++){
        bp_pool[i].NO = i;
        bp_pool[i].next = (i == NR_BP? NULL : &bp_pool[i+1]);
    }
    
    head = NULL;
    free_ = bp_pool;
}

BP* new_bp();
void free_bp(BP *bp);

BP* new_bp(){
    assert(free_);
    BP* tmp = free_;
    free_ = free_->next;
    tmp->next = head;
    head = tmp;

    return tmp;
}

void bp_watch(char* str){
    BP* bp = new_bp();
    strcpy(bp->symbol, str);
    printf("BreakPoint %d : %s\n", bp->NO, bp->symbol);
}

void free_bp(BP *bp){
    assert(head);
    if(head == bp){
        head = bp->next;
    }
    else{
        BP *tmp = head;
        while(tmp->next != bp){
            tmp = tmp->next;
        }
        tmp->next = bp->next;
    }
    bp->next = free_;
    free_ = bp;
}

void delete_bp(int num){
    assert(num < NR_BP);
    BP* tmp = &bp_pool[num];
    free_bp(tmp);
    printf("The breakpoint %d is deleted.\n", num);
    return;
}

void bp_iterate(){
    if(!head){
        printf("No breakpoint have been set!\n");
        return;
    }
    BP *tmp = head;
    while(tmp){
        printf("num        what\n");
        printf("%d        %s\n", tmp->NO, tmp->symbol);
        tmp = tmp -> next;
    }
}

void bp_difftest(char* str){
    if(!head){
        return;
    }
    BP *tmp = head;
    while(tmp){
        if(strcmp(tmp->symbol, str) == 0){
            printf("The symbol is %s\n", tmp->symbol);
            printf("The current instruct address is %s\n", str);
            nemu_state.state = NEMU_STOP;
        }
        tmp = tmp->next;
    }
}

