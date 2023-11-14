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

const char *regsl[] = {
  "$0", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
  "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
  "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
  "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
};

// this should be enough
static char buf[65536] = {};
static char code_buf[65536 + 512] = {}; // a little larger than `buf`
static char *code_format =
"#include <stdio.h>\n"
"int main() { "
"  unsigned $$0 = 0,$ra = 0,$sp = 0,$gp = 0,$tp = 0,$t0 = 0,$t1 = 0,$t2 = 0,$s0 = 0,$s1 = 0,$a0 = 0,$a1 = 0,$a2 = 0,$a3 = 0,$a4 = 0,$a5 = 0,$a6 = 0,$a7 = 0,$s2 = 0,$s3 = 0,$s4 = 0,$s5 = 0,$s6 = 0,$s7 = 0,$s8 = 0,$s9 = 0,$s10 = 0,$s11 = 0,$t3 = 0,$t4 = 0,$t5 = 0,$t6 = 0,$pc = 0x80000000;"
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
  if(num_token > 60000){
    buf[place++] = '1';
    return 1;
  }
  uint32_t n = 0,a=0,b=0;
  switch(rand()%3){
  	case 0:
      int choose_1 = rand()%33;
      if(choose_1 == 32)
  		{
        n = rand()%64;
  	    int n_len = countDigits(n);
        snprintf(buf + place,n_len+1,"%u",n);
        place += n_len;
        num_token++;
        break;
      }
      else
      {
        n = 0;
        int nlen = strlen(regsl[choose_1]);
        snprintf(buf + place,nlen+3,"$%s",regsl[choose_1]);
        place += (nlen + 1);
        num_token++;
        break;
      }
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
