#include "common.h"
#include <isa.h>

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>
#include "memory/vaddr.h"

enum {
  TK_NOTYPE = 256, 
  /* TODO: Add more token types */
  TK_DECIMAL, TK_HEXADECIMAL, TK_REG, TK_NEG, TK_EQ, TK_NE, TK_AND, TK_DEREF
};

static struct rule {
  const char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},    // spaces
  {"0[xX][0-9a-fA-F]+", TK_HEXADECIMAL},
  {"[0-9]+", TK_DECIMAL},   // one decimal number
  {"\\$[a-z]*[0-9]*", TK_REG},
  {"\\+", '+'},         // plus
  {"\\-", '-'},         // minus
  {"\\*", '*'},         // multiply
  {"\\/", '/'},         // divide
  {"\\(", '('},        // left parenthesis
  {"\\)", ')'},        // right parethesis
  {"==", TK_EQ},
  {"!=", TK_NE},
  {"&&", TK_AND}
};

#define NR_REGEX ARRLEN(rules)

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

static Token tokens[32] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

int check_parentheses(Token *start, Token *end);
Token *calc(Token *start, Token *end);
void check_Negtive(Token *start, Token *end);
word_t op_Deref(vaddr_t args);


static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);

        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        
        switch (rules[i].token_type) {
          case '+':
          case '-':
          case '*':
          case '/':
          case '(':
          case ')':
          case TK_DECIMAL:
          case TK_HEXADECIMAL:
          case TK_REG:
          case TK_EQ:
          case TK_NE:
          case TK_AND:
            tokens[nr_token].type = rules[i].token_type;
            //已知字符串长度的情况下，strncpy比较适用
            strncpy(tokens[nr_token].str, substr_start, substr_len);
            tokens[nr_token].str[substr_len] = '\0';
            nr_token++;
            break;
          case TK_NOTYPE:
            break;
        }

        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}

int eval(Token *start, Token *end) {
  if (start > end) {
    /* Bad expression */
    panic("Error Expression, Token start address is greater than the token end.");
  }
  else if (start == end) {
    /* Single token.
     * For now this token should be a number.
     * Return the value of the number.
     */
     if(start->type == TK_HEXADECIMAL){
      vaddr_t addr;
      sscanf(start->str, "%x", &addr);
      return addr;
     }
     else if(start->type == TK_REG){
      bool flag;
      word_t reg_value = isa_reg_str2val(start->str, &flag);
      if(!flag){
        panic("Error: The register name is not defined.");
      }
      return reg_value;
     }
     return atoi(start->str);
  }
  else if (check_parentheses(start, end) == true) {
    /* The expression is surrounded by a matched pair of parentheses.
     * If that is the case, just throw away the parentheses.
     */
    return eval(start + 1, end - 1);
  }
  else {
    /* We should do more things here. */
    int val1=0;
    int val2=0;
    Token *op = calc(start, end);
    if(op->type != TK_NEG && op->type != TK_DEREF && op->type != TK_REG){
      val1 = eval(start, op-1);
    }
    val2 = eval(op+1, end);

    switch(op->type){
      case '+': return val1 + val2;
      case '-': return val1 - val2;
      case '*': return val1 * val2;
      case '/': return val1 / val2;
      case TK_NEG: return val2*-1;
      case TK_DEREF: return op_Deref(val2);
      case TK_REG: return val2;
      case TK_EQ: return val1 == val2;
      case TK_NE: return val1 != val2;
      case TK_AND: return val1 && val2;
      default: panic("Error Expression, the op is not matched any of the rules.");
    }

  }
}

Token *calc(Token *start, Token *end){
  int count = 0;
  // value sign is intended to define the priority of the op
  int sign = 0;
  Token *op = NULL;

  for(Token *sim = start;sim <= end;sim++){
    if(sim->type == '('){
      count++;
      continue;
    }
    if(sim->type == ')'){
      count--;
      continue;
    }
    if(count != 0){
      continue;
    }
    if(sign <= 2 && (sim->type == '+' || sim->type == '-')){
      op = sim;
      sign = 2;
    }
    else if(sign <= 1 && (sim->type == '*' || sim->type =='/')){
      op = sim;
      sign = 1;
    }
    else if(sign == 0 && (sim->type == TK_NEG || sim->type == TK_DEREF || sim->type == TK_REG || 
            sim->type == TK_EQ || sim->type == TK_NE || sim->type == TK_AND)){
      op = sim;
    }
  }
  return op;
}

void check_Negtive(Token *start, Token *end){
  for(;start<=end;start++){
    if(start->type == '-' && (start-1)->type != TK_DECIMAL && (start-1)->type != ')')
    {
      start->type=TK_NEG;
    }
  }
  return;
}

void check_Deref(Token *start, Token *end){
  for(;start <= end;start++){
    if(start->type == '*' && (start-1)->type != TK_DECIMAL && (start-1)->type != TK_HEXADECIMAL && (start-1)->type !=')'){
      start->type = TK_DEREF;
    }
  }
  return;
}

word_t op_Deref(vaddr_t addr){
  return vaddr_read(addr, 4);
}

/* 1. Check if the expression is surrounded by a matched pair of parentheses.
 * 2. Check if all the parentheses matched correctly.
 */
int check_parentheses(Token *start, Token *end){
  int count = 0;
  int sign = 0;

  if(start->type != '(' || end->type != ')'){
    return false;
  }
  for( Token *sym = start;sym < end;sym++){
    if(sym->type == '('){
      count++;
    }
    else if(sym->type == ')'){
      count--;
    }
    if(count == 0){
      //最前面的括号和中间的括号匹配了
      sign =1;
    }
  }
  if(count==1 && sign==1){
    return false;
  }
  if(count==1 && sign==0){
    return true;
  }
  // 括号错误匹配
  panic("Error Expression, the parentheses is not matched.");

}

word_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  /* TODO: Insert codes to evaluate the expression. */

  // check if there has '-' meaning of negtive value and change it's type
  check_Negtive(tokens, tokens + nr_token - 1);
  check_Deref(tokens, tokens + nr_token - 1);

  int num = eval(tokens, tokens + nr_token - 1);
  return num;
}
