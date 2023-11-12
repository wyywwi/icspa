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
    free_ = free_->next;
    now->next = NULL;
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
      printf("%d %s\n",now->NO,now->exp);
      now = now -> next;
    }
  }
}
/* TODO: Implement the functionality of watchpoint */

