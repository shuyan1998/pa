#include "sdb.h"
#include "utils.h"
#include <stdlib.h>

#define NR_WP 32

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;


  /* TODO: Add more members if necessary */
  char expr[20];
  word_t old;

} WP;

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = (i == NR_WP - 1 ? NULL : &wp_pool[i + 1]);
  }

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */
WP* new_wp();
void free_wp(WP *wp);

WP* new_wp(){
  assert(free_);
  WP* tmp = free_;
  free_ = free_->next;
  tmp->next = head;
  head = tmp;
  return tmp;
}

void free_wp(WP *wp){
  assert(head);
  WP *tmp = head;
  if(head == wp){
    if(head->next){
      head = head->next;
    }
    else{
      head = NULL;
    }
  }
  else{
    int flag = 0;
    while(tmp->next != wp){
      tmp = tmp->next;
      flag =1;
    }
    if(!flag){head->next = NULL;}
  }
  wp->next = free_;
  free_ = wp;
}

void wp_watch(char *expr, word_t old){
  WP *new = new_wp();
  strcpy(new->expr, expr);
  new->old = old;
  printf("Watchpoint %d: %s\n", new->NO, expr);
}

void wp_iterate(){
  if(!head){
    printf("There is no watchpoint set.\n");
    return;
  }
  WP* tmp = head;
  while(tmp){
    printf("num        what\n");
    printf("%d        %s\n", tmp->NO, tmp->expr);
    tmp = tmp->next;
  }
}

void wp_delete(int num){
  assert(num < NR_WP);
  WP* wp = &wp_pool[num];
  free_wp(wp);

  return;
}

void wp_difftest(){
  WP *tmp = head;
  word_t new;
  bool success;
  while(tmp){
    new = expr(tmp->expr, &success);
    if(new != tmp->old){
      nemu_state.state = NEMU_STOP;
      printf("Watchpoint %d: %s\n \
        Old value = %u\n \
        New value = %u\n",
        tmp->NO, tmp->expr, tmp->old, new);
        tmp->old = new;
    }
    tmp = tmp->next;
  }
}