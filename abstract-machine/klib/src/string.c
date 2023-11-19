#include <klib.h>
#include <klib-macros.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
  int64_t count = 0;
  while(*(s + count) != '\0')count++;
  return count;
}

char *strcpy(char *dst, const char *src) {
  int64_t count = 0;
  while(*(src + count) != '\0'){
    *(dst + count) = *(src + count);
    count++;
  }
  *(dst + count) = '\0';
  return dst;
}

char *strncpy(char *dst, const char *src, size_t n) {
  int64_t count = 0;
  while(*(src + count) != '\0' && count < n){
    *(dst + count) = *(src + count);
    count++;
  }
  *(dst + count) = '\0';
  return dst;
}

char *strcat(char *dst, const char *src) {
  int64_t count1 = 0,count2 = 0;
  while(*(dst+count1) != '\0')count1++;
  while(*(src+count2) != '\0'){
    *(dst + count1 + count2) = *(src + count2);
    count2++;
  }
  *(dst + count1 + count2) = '\0';
  return dst;
}

int strcmp(const char *s1, const char *s2) {
  //for (; *s1==*s2 && *s1; s1++, s2++);
  while(*s1 == *s2 && *s1){s1++;s2++;}
	return *(unsigned char *)s1 - *(unsigned char *)s2;
}

int strncmp(const char *s1, const char *s2, size_t n) {
  const unsigned char *l=(void *)s1, *r=(void *)s2;
	if (!n--) return 0;
  while(*l && *r && n && *l == *r){l++;r++;n--;}
	return *l - *r;
}

void *memset(void *s, int c, size_t n) {
  unsigned char *st = (unsigned char *)s;
  for(size_t i = 0;i < n;i++){
    * (st + i) = (unsigned char)c;
  }
  return s;
}

void *memmove(void *dst, const void *src, size_t n) {
  if(dst < src){
    unsigned char *l = dst;
    for(size_t i = 0;i < n;i++){
      * (l + i) = (*((const unsigned char *)src + i));
    }
  }
  else{
    unsigned char *l = dst;
    for(size_t i = 0;i < n;i++){
      * (l + n - i - 1) = (*((const unsigned char *)src + n + i - 1));
    }
  }
  return dst;
}

void *memcpy(void *out, const void *in, size_t n) {
  //panic("Not implemented");
  unsigned char *l = out;
  for(size_t i = 0;i < n;i++){
    * (l + i) = (*((const unsigned char *)in + i));
  }
  return out;
}

int memcmp(const void *s1, const void *s2, size_t n) {
  //panic("Not implemented");
  const unsigned char *l=(void *)s1, *r=(void *)s2;
	if (!n--) return 0;
  while(n && *l == *r){l++;r++;n--;}
	return *l - *r;
}

#endif
