#include "sys/unistd.h"
#include <cstring>
#include <nterm.h>
#include <stdarg.h>
#include <unistd.h>
#include <SDL.h>

char handle_key(SDL_Event *ev);

static void sh_printf(const char *format, ...) {
  static char buf[256] = {};
  va_list ap;
  va_start(ap, format);
  int len = vsnprintf(buf, 256, format, ap);
  va_end(ap);
  term->write(buf, len);
}

static void sh_banner() {
  sh_printf("Built-in Shell in NTerm (NJU Terminal)\n\n");
}

static void sh_prompt() {
  sh_printf("sh> ");
}

static void sh_handle_cmd(const char *cmd) {
  // 复制命令字符串以便解析
  char cmd_copy[256];
  strncpy(cmd_copy, cmd, sizeof(cmd_copy) - 1);
  cmd_copy[sizeof(cmd_copy) - 1] = '\0';

  // remove last '\n'
  char *newline = strchr(cmd_copy, '\n');
  if(newline){
    *newline = '\0';
  }

  char *token = strtok(cmd_copy, " ");
  if (token == NULL) {
    fprintf(stderr, "Invalid command\n");
    return;
  }

  // 构建 argv 数组
  char *argv[64];  // 假设最多有 64 个参数
  int argc = 0;
  argv[argc++] = token;

  while ((token = strtok(NULL, " ")) != NULL) {
    argv[argc++] = token;
    printf("==arg is %s\n", token);
  }
  argv[argc] = NULL;  // 最后一个元素必须是 NULL

  const char *envp[] = {
    "PATH=/bin",
    NULL
  };

  printf("== exec is %s\n", argv[0]);
  //execvp(argv[0], (char **)argv);
  execve(argv[0], (char**)argv, (char**)envp);
}

void builtin_sh_run() {
  sh_banner();
  sh_prompt();

  while (1) {
    SDL_Event ev;
    if (SDL_PollEvent(&ev)) {
      if (ev.type == SDL_KEYUP || ev.type == SDL_KEYDOWN) {
        const char *res = term->keypress(handle_key(&ev));
        if (res) {
          printf("res is %s\n", res);
          sh_handle_cmd(res);
          sh_prompt();
        }
      }
    }
    refresh_terminal();
  }
}
