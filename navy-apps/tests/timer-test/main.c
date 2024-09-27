#include <stdint.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdio.h>
#include <NDL.h>
int main() {
  NDL_Init(0);
  uint32_t us = 0;
  
  while(1){
    us = NDL_GetTicks();
    printf("us is %d\n", us);
    while(us < 500000);
    printf("Hello World from Navy-apps!\n");
  }
  return 0;
}