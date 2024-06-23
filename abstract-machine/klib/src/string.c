#include <klib.h>
#include <klib-macros.h>
#include <stddef.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
  int count = 0;
  while(*s != '\0'){
    s++;
    count++;
  }
  return count;
}

char *strcpy(char *dst, const char *src) {
  char *tmp = dst;
  while(*src != '\0'){
    *tmp = *src;
    tmp++;
    src++;
  }
  *tmp = '\0';
  return dst;
}

char *strncpy(char *dst, const char *src, size_t n) {
  while(n) {
    if(*src != '\0') {
      *dst = *src;
    }
    src++;
    dst++;
    n--;
  }
  return dst;
}

char *strcat(char *dst, const char *src) {
  char *tmp = dst;
  while(*tmp != '\0') {
    tmp++;
  }
  strcpy(tmp, src);
  return dst;
}

int strcmp(const char *s1, const char *s2) {
  while(*s1 != '\0' && *s2 != '\0'){
    if(*s1 == *s2){
      s1++;
      s2++;
    }
    else {
      return -1;
    }
    if(*s1 == '\0' && *s2 == '\0'){
      return 0;
    }
  }
  return -1;
}

int strncmp(const char *s1, const char *s2, size_t n) {
  while(n) {
    if(*s1 == *s2){
      s1++;
      s2++;
    }
    else {
      return -1;
    }
    n--;
  }
  return 0;
}

void *memset(void *s, int c, size_t n) {
  char *tmp = s;
  while(n) {
    *tmp = c;
    tmp++;
    n--;
  }
  return s;
}

void *memmove(void *dst, const void *src, size_t n) {
  char *tmp_dst = dst;
  const char *tmp_src = src;
  while(n){
    tmp_dst[n-1] = tmp_src[n-1];
    n--;
  }
  return dst;
}

void *memcpy(void *out, const void *in, size_t n) {
  char *tmp_out = out;
  const char *tmp_in = in;
  while(n) {
    *tmp_out = *tmp_in;
    tmp_out++;
    tmp_in++;
    n--;
  }
  return out;
}

int memcmp(const void *s1, const void *s2, size_t n) {
  const char *tmp_s1 = s1;
  const char *tmp_s2 = s2;
  while(n) {
    if(tmp_s1[n-1] != tmp_s2[n-1]){
      return -1;
    }
    n--;
  }
  return 0;
}

#endif
