/***************************************************************************************
* Copyright (c) 2014-2022 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#include <isa.h>

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>
#include <string.h>
#include <memory/paddr.h>

enum {
  TK_NOTYPE = 256, TK_EQ,
  TK_PLUS,TK_SUB,TK_STAR,
  TK_DIV,TK_LP,TK_RP,
  TK_NUM,TK_HEX,TK_REG,
  TK_NEQ,TK_REF,TK_AND,
  TK_NEG,
  /* TODO: Add more token types */

};

static struct rule {
  const char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */
  {" +", TK_NOTYPE},// spaces
  {"[*]", TK_STAR},
  {"[-]", TK_SUB},
  {"[+]", TK_PLUS},         // plus
  {"==", TK_EQ}, // equal
  {"!=",TK_NEQ},
  {"&&",TK_AND},
  {"[/]", TK_DIV},
  {"[(]", TK_LP},
  {"[)]", TK_RP},
  {"\\$(\\$0|ra|sp|gp|tp|t[0-6]|s[0-9]|a[0-7]|s10|s11",TK_REG},
  {"(0x|0X)[0-9a-fA-F]+",TK_HEX},
  {"0|[1-9][0-9]*", TK_NUM},
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

static Token tokens[65536] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

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
          case TK_NOTYPE:
            continue;
          default:
            tokens[nr_token].type = rules[i].token_type;
            strncpy(tokens[nr_token].str,substr_start,substr_len);
            tokens[nr_token].str[substr_len] = '\0';
            nr_token ++;
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

static inline bool check_parentheses(int p,int q){
  if(tokens[p].type != TK_LP || tokens[q].type != TK_RP)return false;
  else{
    int count = 0;
    for(int i = p ; i <= q ; i++){
      if(tokens[i].type == TK_LP)count++;
      else if(tokens[i].type == TK_RP)count--;
      if(count == 0 && i!= q)return false;
    }
  }
  return true;
}

static int get_order(int type,int count){
  int order = 0;
  switch (type)
  {
  case TK_STAR:
  case TK_DIV:
    order = 4; 
    break;
  case TK_PLUS:
  case TK_SUB:
    order = 3;
    break;
  case TK_EQ:
  case TK_NEQ:
    order = 2;
    break;
  case TK_AND:
    order = 1;
    break;
  default:
    order = 5;
    break;
  }
  return order + count*10;
}

word_t eval(int p,int q){
  if(p > q){
    for(int i = 0;i<nr_token;i++){
      printf("%s",tokens[i].str);
    }
    panic("Bad Expression p = %d,q = %d",p,q);
  }
  else if(q - p == 1){
    if(tokens[p].type == TK_REF){
      word_t addr = eval(q,q);
      word_t value = paddr_read(addr,4);
      return value;
    }
    if(tokens[p].type == TK_NEG){
      word_t val = eval(q,q);
      return (-val);
    }
  }
  else if(p == q){
    if(tokens[p].type == TK_NUM){
      word_t number;
      sscanf(tokens[p].str,"%u",&number);
      return number;
    }
    else if(tokens[p].type == TK_HEX){
      word_t number;
      sscanf(tokens[p].str,"%x",&number);
      return number;
    }
    else if(tokens[p].type == TK_REG){
      word_t number;
      bool success = true;
      number = isa_reg_str2val(tokens[p].str,&success);
      if(!success) return 0;
      return number;
    }
  }
  else if(check_parentheses(p,q)){
    return eval(p+1,q-1);
  }
  else {
    int op = p - 1,now_order = 65536,count = 0;
    for(int i = p ; i <= q ; i++){
      if(tokens[i].type == TK_LP){
        count++;
        continue;
      }
      if(tokens[i].type == TK_RP){
        count--;
        continue;
      }
      int find_order = get_order(tokens[i].type,count);
      if(find_order <= now_order){
        op = i;
        now_order = find_order;
      }
    }
    if(op == p - 1)assert(0);
    uint32_t val1 = eval(p,op-1);
    uint32_t val2 = eval(op+1,q);
    switch (tokens[op].type)
    {
    case TK_PLUS:
      return val1 + val2;
    case TK_SUB:
      return val1 - val2;
    case TK_DIV:
      return val1 / val2;
    case TK_STAR:
      return val1 * val2;
    case TK_EQ:
      return val1 == val2;
    case TK_NEQ:
      return val1 != val2;
    case TK_AND:
      return val1 && val2;
    default:                                                                                                                                                                                                                                                                                                                                                                                     
      assert(0);
    }
  }
  return 0;
}

word_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }
  *success = true;
  /* TODO: Insert codes to evaluate the expression. */
  //TODO();
  //*success = true;
  for(int i = 0; i < nr_token ; i++){
    if(tokens[i].type == TK_SUB || tokens[i].type == TK_STAR)
    {
      if(i == 0 || (tokens[i-1].type != TK_NUM && tokens[i-1].type != TK_REG && tokens[i-1].type != TK_HEX && tokens[i-1].type != TK_RP)){
        if(tokens[i].type == TK_SUB){
          tokens[i].type = TK_NEG;
        }
        else{
          tokens[i].type = TK_REF;
        }
      }
    }
  }
  return eval(0,nr_token - 1);
}
