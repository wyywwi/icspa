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

#include "sdb.h"

#define NR_WP 32

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;
  char exp[65536];  
  uint32_t last_value;
  /* TODO: Add more members if necessary */

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

WP* new_wp(char *exp){
  if(free_ == NULL)return NULL;
  if(head == NULL){
    head = free_;
    strcpy(head->exp,exp);
    bool su = true;
    head->last_value = expr(head->exp,&su);
    free_ = free_->next;
    head->next = NULL;
    return head;
  }
  else{
    WP * now = head;
    while(now->next != NULL){
      now = now->next;
    }
    now->next = free_;
    strcpy(now->next->exp,exp);
    bool su = true;
    head->last_value = expr(head->exp,&su);
    free_ = free_->next;
    now->next->next = NULL;
    return now->next;
  }
}

void free_wp(WP *wp){
  if(wp == NULL)return;
  if(head == wp){
    head = wp->next;
    wp->next = free_;
    free_ = wp;
  }
  else{
    WP *now = head;
    while(now->next != wp){
      now = now->next;
    }
    now->next = wp->next;
    wp ->next = free_;
    free_ = wp;
  }
}

void print_wp(){
  if(head == NULL){
    Log("no watching points");
  }
  else{
    WP *now = head;
    while(now != NULL){
      printf("%d %s %u\n",now->NO,now->exp,now->last_value);
      now = now -> next;
    }
  }
}

void delete_wp(int N){
  if(head == NULL)return;
  WP * now = head;
  while(now != NULL){
    if(now->NO == N){
      free_wp(now);
      return;
    }
    now = now->next;
  }
}

bool check_wp_diff(int *n){
  bool diff = false;
  WP *now = head;
  while(now != NULL){
    bool su = true;
    int now_value = expr(now->exp,&su);
    if(now_value != now->last_value){
      now->last_value = now_value;
      diff = true;
      *n = now -> NO;
    }
    now = now->next;
  }
  return diff;
}
/* TODO: Implement the functionality of watchpoint */

