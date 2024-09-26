#include <stdint.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdio.h>
#include <NDL.h>
int main() {
  uint32_t us = 0;
  
  while(1){
    us = NDL_GetTicks();
    printf("us is %d\n", us);
    while(us < 500000);
    printf("Hello World from Navy-apps!\n");
  }
  return 0;

//   AM_TIMER_RTC_T rtc;
//   int sec = 1;
//   while (1) {
//     while(io_read(AM_TIMER_UPTIME).us / 1000000 < sec) ;
//     rtc = io_read(AM_TIMER_RTC);
//     printf("%d-%d-%d %02d:%02d:%02d GMT (", rtc.year, rtc.month, rtc.day, rtc.hour, rtc.minute, rtc.second);
//     if (sec == 1) {
//       printf("%d second).\n", sec);
//     } else {
//       printf("%d seconds).\n", sec);
//     }
//     sec ++;
//   }
}