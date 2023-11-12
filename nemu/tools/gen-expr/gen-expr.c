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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>

// this should be enough
static char buf[65536] = {};
static char code_buf[65536 + 128] = {}; // a little larger than `buf`
static char *code_format =
"#include <stdio.h>\n"
"int main() { "
"  unsigned result = %s; "
"  printf(\"%%u\", result); "
"  return 0; "
"}";
static int place = 0;

static int countDigits(uint32_t num) {
    // 处理0的情况
    if (num == 0) {
        return 1;
    }

    // 初始化计数器
    int count = 0;

    // 计算位数
    while (num != 0) {
        num /= 10;
        count++;
    }
    return count;
}

static int num_token = 0;
static unsigned int gen_rand_expr() {
  if(num_token > 2000){
    buf[place++] = '1';
    return 1;
  }
  uint32_t n = 0,a=0,b=0;
  switch(rand()%3){
  	case 0:
  		n = rand()%INT32_MAX;
  	  int n_len = countDigits(n);
      snprintf(buf + place,n_len+1,"%u",n);
      place += n_len;
      num_token++;
      break;
  	case 1:
      buf[place++] = '(';
      num_token++;
      n = gen_rand_expr();
      buf[place++] = ')';
      num_token++;
      break;
  	case 2:
      int choose = random();
      a = gen_rand_expr();
      int now_place = place;
      num_token++;
      place++;
      b = gen_rand_expr();
      if(b == 0)
      {
        switch(choose%4)
        {
          case 0:buf[now_place] = '+';n = a + b;break;
          case 1:buf[now_place] = '-';n = a - b;break;
          case 2:buf[now_place] = '*';n = a * b;break;
          case 3:buf[now_place] = '/';n = a / b;break;
        } 
      }
      else{
        switch(choose%3)
        {
          case 0:buf[now_place] = '+';n = a + b;break;
          case 1:buf[now_place] = '-';n = a - b;break;
          case 2:buf[now_place] = '*';n = a * b;break;
        }
      }
      break;
  }
  return n;
}

int main(int argc, char *argv[]) {
  int seed = time(0);
  srand(seed);
  int loop = 1;
  if (argc > 1) {
    sscanf(argv[1], "%d", &loop);
  }
  int i;
  for (i = 0; i < loop; i ++) {
    place = 0;
    num_token = 0;
    gen_rand_expr();
    buf[place] = '\0';

    sprintf(code_buf, code_format, buf);

    FILE *fp = fopen("/tmp/.code.c", "w");
    assert(fp != NULL);
    fputs(code_buf, fp);
    fclose(fp);

    int ret = system("gcc /tmp/.code.c -o /tmp/.expr");
    if (ret != 0) continue;

    fp = popen("/tmp/.expr", "r");
    assert(fp != NULL);

    int result;
    ret = fscanf(fp, "%d", &result);
    pclose(fp);

    printf("%u %s\n", result, buf);
  }
  return 0;
}
