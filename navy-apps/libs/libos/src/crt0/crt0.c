#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

int main(int argc, char *argv[], char *envp[]);
extern char **environ;

void call_main(uintptr_t *args) {
  printf("call_main\n");
   // 从参数中获取argc的值
  int argc = (int)args[0];
  printf("argc = %d\n", argc);
  
  // 从参数中获取argv的地址
  char **argv = (char **)(args + 1);
  printf("argv[%d] = %s\n", 0, argv[0]);
  
  for(int i = 0; i < argc; i++) {
    printf("argv[%d] = %s\n", i, argv[i]);
  }
 
  // 从参数中获取envp的地址
  char **envp = (char **)(args + argc + 1);
  for(int i = 0; envp[i] != NULL; i++){
    printf("envp[%d] = %s\n", i, envp[i]);
  }
 
  // 设置environ为envp
  environ = envp;
 
  // 调用main函数
  exit(main(argc, argv, envp));
  assert(0);
}