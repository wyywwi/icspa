#include <am.h>
#include <nemu.h>
void __am_timer_init() {
  outl(0xa0000048,0);
  outl(0xa0000048 +4,0);
}

void __am_timer_uptime(AM_TIMER_UPTIME_T *uptime) {
  uptime->us = inl(0xa0000048 +4);
  uptime->us <<=32;
  uptime->us += inl(0xa0000048);
}

void __am_timer_rtc(AM_TIMER_RTC_T *rtc) {
  rtc->second = 0;
  rtc->minute = 0;
  rtc->hour   = 0;
  rtc->day    = 0;
  rtc->month  = 0;
  rtc->year   = 1900;
}
