#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

/*
  There has five main steps to implement printf and other functions below:
  1. scan and get the string after '%'
  2. get the value of the argument by va_arg
  3. tranform the value to string
  4. print or store the string to buffer
  5. add count
*/
int printf(const char *fmt, ...) {
  va_list arg_ptr;
  va_start(arg_ptr, fmt);
  int count = 0;

  while(*fmt != '\0'){
    if(*fmt == '%'){
      fmt++;
      char str[10000];
      switch(*fmt){
        case 'd':{
          int value = va_arg(arg_ptr, int);
          itoa(value, str);
          break;
        }
        case 's':{
          char *value = va_arg(arg_ptr, char*);
          strcpy(str, value);
          break;
        }
      }
      for(int i = 0; i < strlen(str); i++){
        putch(str[i]);
      }
      count += strlen(str);
    }
    else{
      putch(*fmt);
      count++;
    }
    fmt++;
  }
  return count;
}

int sprintf(char *out, const char *fmt, ...) {
  va_list arg_ptr;
  va_start(arg_ptr, fmt);
  int count = 0;

  while(*fmt != '\0') {
    if(*fmt == '%') {
      fmt++;
      char str[20];
      switch(*fmt){
        case 'd':{
          int value = va_arg(arg_ptr, int);
          itoa(value, str);
          strcpy(out, str);
          break;
        }
        case 's':{
          char *value = va_arg(arg_ptr, char*);
          strcpy(out, value);
          strcpy(str, value);
          break;
        }
      }
      out += strlen(str);
      count += strlen(str);
    }
    else{
      strncpy(out, fmt , 1);
      out++;
      count ++;
    }
    fmt++;
  }
  va_end(arg_ptr);
  return count;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  int count = 0;

  while(*fmt != '\0'){
    if(*fmt == '%'){
      fmt++;
      char str[20];
      switch(*fmt) {
        case 'd':{
          int value = va_arg(ap, int);
          itoa(value, str);
          strcpy(out, str);
          break;
        }
        case 's':{
          char *value = va_arg(ap, char*);
          strcpy(str, value);
          strcpy(out, value);
          break;
        }
      }
      out += strlen(str);
      count += strlen(str);
    }
    else{
      strncpy(out, fmt , 1);
      out++;
      count ++;
    }
    fmt++;
  }
  va_end(ap);
  return count;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  va_list arg_ptr;
  va_start(arg_ptr, fmt);
  int count = 0;

  while(*fmt != '\0' && count < n - 1){
    if(*fmt == '%'){
      fmt++;
      char str[20];
      switch(*fmt){
        case 'd':{
          int value = va_arg(arg_ptr, int);
          itoa(value, str);
          break;
        }
        case 's':{
         char *value = va_arg(arg_ptr, char*);
         strcpy(str, value);
         break;
        }
      }
      if(strlen(str) > n - count){ 
        int cp_sz = n - 1 - count;
        strncpy(out, str, cp_sz);
        out += cp_sz;
        count += strlen(str);
      }
      else{
        strcpy(out, str);
        out += strlen(str);
        count += strlen(str);
      }
    }
    else{
      strncpy(out, fmt, 1);
      out++;
      count++;
    }
    fmt++;
  }
  va_end(arg_ptr);
  return count;
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  int count = 0;

  while(*fmt != '\0' && count < n - 1){
    if(*fmt == '%'){
      fmt++;
      char str[20];
      switch(*fmt) {
        case 'd':{
          int value = va_arg(ap, int);
          itoa(value, str);
          break;
        }
        case 's':{
          char *value = va_arg(ap, char*);
          strcpy(str, value);
          break;
        }
      }
      if(strlen(str) > n - count){ 
        int cp_sz = n - 1 - count;
        strncpy(out, str, cp_sz);
        out += cp_sz;
        count += strlen(str);
      }
      else{
        strcpy(out, str);
        out += strlen(str);
        count += strlen(str);
      }
    }
    else{
      strncpy(out, fmt, 1);
      out++;
      count++;
    }
  }
  va_end(ap);
  return count;
}

#endif
