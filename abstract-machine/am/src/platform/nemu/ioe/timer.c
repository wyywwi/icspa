#include <am.h>
#include <nemu.h>
uint64_t static now_time;
void __am_timer_init() {
  uint64_t high = inb(RTC_ADDR);
  uint64_t low = inb(RTC_ADDR + 4);
  now_time = (high << 32) + low;
}

void __am_timer_uptime(AM_TIMER_UPTIME_T *uptime) {
  uint64_t high = inb(RTC_ADDR);
  uint64_t low = inb(RTC_ADDR + 4);
  uptime->us = now_time - ((high<<32)+low);
}

void __am_timer_rtc(AM_TIMER_RTC_T *rtc) {
  rtc->second = 0;
  rtc->minute = 0;
  rtc->hour   = 0;
  rtc->day    = 0;
  rtc->month  = 0;
  rtc->year   = 1900;
}
