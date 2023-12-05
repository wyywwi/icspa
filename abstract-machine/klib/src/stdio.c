// #include <am.h>
// #include <klib.h>
// #include <klib-macros.h>
// #include <stdarg.h>

// #if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

// int printf(const char *fmt, ...) {
//   char buffer[1024];
//   va_list arg;
//   va_start(arg, fmt);
//   int done = vsprintf(buffer, fmt, arg);  // 将格式化的内容(字符串)保存在buffer中
//   putstr(buffer);
//   va_end(arg);
//   return done;
// }

// int vsprintf(char *out, const char *fmt, va_list ap) {
//   return vsnprintf(out,1024, fmt, ap);
// }

// int sprintf(char *out, const char *fmt, ...) {
//   int ret;
// 	va_list ap;
// 	va_start(ap, fmt);
// 	ret = vsprintf(out, fmt, ap);
// 	va_end(ap);
// 	return ret;
// }

// int snprintf(char *out, size_t n, const char *fmt, ...) {
//   int ret;
// 	va_list ap;
// 	va_start(ap, fmt);
// 	ret = vsnprintf(out, n, fmt, ap);
// 	va_end(ap);
// 	return ret;
// }

// int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
//   int count1 = 0,count2 = 0;
//   while(*(fmt + count2) != '\0' && count2 < n){
//     if(*(fmt + count2) == '%'){
//       if(*(fmt + count2 + 1) == 'd'){
//         int arg = va_arg(ap,int);
//         int temp = arg;
//         int digits = 0;
//         if(temp == 0){
//           digits = 1;
//         }else{
//           while(temp != 0){
//             temp/= 10;
//             digits ++;
//           }
//         }
//         if(count1 + digits < n){
//           int disvisor = 1;
//           for(int i = 0;i<digits -1;i++){
//             disvisor *= 10;
//           }
//           while(disvisor != 0){
//             int digit = arg/disvisor;
//             *(out + count1) = '0' + digit;
//             count1++;
//             arg %= disvisor;
//             disvisor /= 10;
//           }
//         }
//       }
//       else if(*(fmt + count2 + 1) == 's'){
//         char *arg = va_arg(ap,char *);
//         while(*arg != '\0' && count1 < n){
//           *(out + count1) = *arg;
//           count1++;
//           arg++;
//         }
//       }
//       else if(*(fmt + count2 + 1) == 'c'){
//         int arg = va_arg(ap,int);
//         *(out + count1) = arg;
//         count1++;
//       }
//       count2 += 2;
//     }
//     else{
//       *(out + count1) = *(fmt + count2);
//       count1++;
//       count2++;
//     } 
//   }
//   *(out + count1) = '\0';
//   return count1;
// }

// #endif
#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int printf(const char *fmt, ...) {
  char buf[1000];
  va_list ap;
  va_start(ap,fmt);
  int length=vsprintf(buf,fmt,ap);
  va_end(ap);
  putstr(buf);
  return length;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  char *temp=out;
  while(*fmt!='\0'){
    if(*fmt!='%'){
      *temp++=*fmt++;
    }
    else{
      fmt++;
      switch(*fmt){
        case 's':{
          char *str=va_arg(ap,char*);
          while(*str!='\0'){
            *temp++=*str++;
          }
          break;
        }
        case 'd':{
          int n=va_arg(ap,int);
          if(n==0){
            *temp++='0';
            break;
          }
          if(n<0){
            *temp++='-';
            n=-n;
          }
          char buf[11];
          int i=0;
          while(n!=0){
            buf[i++]=n%10+'0';
            n=n/10;
          }
          for(int j=i-1;j>=0;j--){
            *temp++=buf[j];
          }
          break;
        }
        case 'x':{
          int n=va_arg(ap,int);
          if(n==0){
            *temp++='0';
            *temp++='x';
            *temp++='0';
            break;
          }
          if(n<0){
            *temp++='-';
            n=-n;
          }
          char buf[12];
          int i=0;
          while(n!=0){
            int a=n%16;
            if(a<10)
              buf[i++]=a+'0';
            else
              buf[i++]=a-10+'a';
            n=n/16;
          }
          *temp++='0';
          *temp++='x';
          for(int j=i-1;j>=0;j--){
            *temp++=buf[j];
          }
          break;
        }
      }
      fmt++;
    }
  }
  *temp='\0';
  return temp-out;
}

int sprintf(char *out, const char *fmt, ...) {
  va_list ap;
  va_start(ap,fmt);
  int length=vsprintf(out,fmt,ap);
  va_end(ap);
  return length;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  return 0;
}

#endif