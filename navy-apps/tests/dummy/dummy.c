#include <stdint.h>
#include <stdlib.h>

#ifdef __ISA_NATIVE__
#error can not support ISA=native
#endif

#define SYS_yield 1
extern int _syscall_(int, uintptr_t, uintptr_t, uintptr_t);

int main() {
  int* ptr = (int*)malloc(10 * sizeof(int));
  for(int i = 0; i < 10; i++) {
    ptr[i] = i;
  }
  return _syscall_(SYS_yield, 0, 0, 0);
}
