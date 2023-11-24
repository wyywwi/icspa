#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int printf(const char *fmt, ...) {
  va_list args;
    va_start(args, fmt);

    int output_count = 0;
    const char* format_ptr = fmt;

    while (*format_ptr != '\0') {
        if (*format_ptr == '%') {
            format_ptr++;
            if (*format_ptr == 'd') {
                // Handle %d format specifier
                int num = va_arg(args, int);
                char str[20];
                sprintf(str, "%d", num);
                char* str_ptr = str;
                while (*str_ptr != '\0') {
                    putch(*str_ptr);
                    str_ptr++;
                    output_count++;
                }
            } else if (*format_ptr == 's') {
                // Handle %s format specifier
                char* str = va_arg(args, char*);
                char* str_ptr = str;
                while (*str_ptr != '\0') {
                    putch(*str_ptr);
                    str_ptr++;
                    output_count++;
                }
            } else if (*format_ptr == 'c'){
                char c = va_arg(args,int);
                putch(c);
            }
        } else {
            putch(*format_ptr);
            output_count++;
        }

        format_ptr++;
    }

    va_end(args);
    return output_count;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  return vsnprintf(out, INT32_MAX, fmt, ap);
}

int sprintf(char *out, const char *fmt, ...) {
  int ret;
	va_list ap;
	va_start(ap, fmt);
	ret = vsprintf(out, fmt, ap);
	va_end(ap);
	return ret;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  int ret;
	va_list ap;
	va_start(ap, fmt);
	ret = vsnprintf(out, n, fmt, ap);
	va_end(ap);
	return ret;
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  int count1 = 0,count2 = 0;
  while(*(fmt + count2) != '\0' && count2 < n){
    if(*(fmt + count2) == '%'){
      if(*(fmt + count2 + 1) == 'd'){
        int arg = va_arg(ap,int);
        int temp = arg;
        int digits = 0;
        if(temp == 0){
          digits = 1;
        }else{
          while(temp != 0){
            temp/= 10;
            digits ++;
          }
        }
        if(count1 + digits < n){
          int disvisor = 1;
          for(int i = 0;i<digits -1;i++){
            disvisor *= 10;
          }
          while(disvisor != 0){
            int digit = arg/disvisor;
            *(out + count1) = '0' + digit;
            count1++;
            arg %= disvisor;
            disvisor /= 10;
          }
        }
      }
      else if(*(fmt + count2 + 1) == 's'){
        char *arg = va_arg(ap,char *);
        while(*arg != '\0' && count1 < n){
          *(out + count1) = *arg;
          count1++;
          arg++;
        }
      }
      count2 += 2;
    }
    else{
      *(out + count1) = *(fmt + count2);
      count1++;
      count2++;
    } 
  }
  *(out + count1) = '\0';
  return count1;
}

#endif
