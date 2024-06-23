#ifndef __SDB_H__
#define __SDB_H__

#include <common.h>

typedef struct watchpoint WP;

word_t expr(char *e, bool *success);
WP* new_wp();
void free_wp(WP *wp);
void wp_watch(char *expr, word_t old);
void wp_iterate();
void wp_delete(int num);
void wp_difftest();

typedef struct breakpoint BP;
BP* new_bp();
void free_bp(BP *bp);
void delete_bp(int num);
void bp_watch(char *str);
void bp_iterate();
void bp_difftest();

#endif