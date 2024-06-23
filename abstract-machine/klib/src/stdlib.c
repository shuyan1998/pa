#include <am.h>
#include <klib.h>
#include <klib-macros.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)
static unsigned long int next = 1;

int rand(void) {
  // RAND_MAX assumed to be 32767
  next = next * 1103515245 + 12345;
  return (unsigned int)(next/65536) % 32768;
}

void srand(unsigned int seed) {
  next = seed;
}

int abs(int x) {
  return (x < 0 ? -x : x);
}

int atoi(const char* nptr) {
  int x = 0;
  while (*nptr == ' ') { nptr ++; }
  while (*nptr >= '0' && *nptr <= '9') {
    x = x * 10 + *nptr - '0';
    nptr ++;
  }
  return x;
}

char *itoa(int n, char* str) {
  // transform n to it's Two's complement if it is a negtive integer
  int flag = 0;
  if(n<0) {
    if(n == -2147483648){
      strcpy(str, "-2147483648");
      return str;
    }
    n = n * (-1);
    flag = 1;
  }
  if(n == 0){
    str[0] = '0';
    str[1] = '\0';
    return str;
  }

  char arr1[20];
  char arr2[20];
  int i = 0, j = 0;
  while(n >= 1) {
    arr1[i++] = n % 10 + '0';
    n /= 10;
    if(n < 1){
      i--;
    }
  }
  if(flag == 1){
    arr2[j++] = '-';
  }

  while(i >= 0) {
    arr2[j++] = arr1[i--];
  }
  arr2[j] = '\0';

  while(j>=0) {
    str[j] = arr2[j];
    j--;
  }

  return str;
}

void *malloc(size_t size) {
  // On native, malloc() will be called during initializaion of C runtime.
  // Therefore do not call panic() here, else it will yield a dead recursion:
  //   panic() -> putchar() -> (glibc) -> malloc() -> panic()
#if !(defined(__ISA_NATIVE__) && defined(__NATIVE_USE_KLIB__))
  panic("Not implemented");
#endif
  return NULL;
}

void free(void *ptr) {
}

#endif
