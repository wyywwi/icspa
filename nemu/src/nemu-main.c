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

#include <common.h>

void init_monitor(int, char *[]);
void am_init_monitor();
void engine_start();
int is_exit_status_bad();
int expr(char *);

int main(int argc, char *argv[]) {
  /* Initialize the monitor. */
#ifdef CONFIG_TARGET_AM
  am_init_monitor();
#else
  init_monitor(argc, argv);
#endif
  int loop = 1000;
  if(argc > 1){
    sscanf(argv[0],"%d",&loop);
  }
  FILE* is_open = freopen("tools/gen-expr/input","r",stdin);
  if(!is_open)panic("error for open");
  for(int i = 0;i<loop;i++){
    uint32_t n;
    char buf[65536];
    int is_sca = scanf("%d %s",&n,buf);
    if(!is_sca)break;
    int result = expr(buf);
    if(result != n)panic("Error at %s and result is %d but need to be %d\n",buf,result,n);
  }
  /* Start engine. */
  engine_start();

  return is_exit_status_bad();
}
