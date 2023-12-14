#include <isa.h>
#include <cpu/cpu.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "common.h"
#include "sdb.h"
#include "utils.h"

static int is_batch_mode = false;

void init_regex();
void init_wp_pool();

/* We use the `readline' library to provide more flexibility to read from stdin. */
static char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  //TODO: when n is bigger than MAX_INSTR_TO_PRINT, the instructions will not print
  cpu_exec(9);
  return 0;
}

static int cmd_q(char *args) {
  nemu_state.state = NEMU_QUIT;
  return -1;
}

static int cmd_help(char *args);

static int cmd_si(char *args){
  //char *arg = strtok(NULL, " ");
  int step;

  if(args == NULL){
    step = 1;
  }
  else{
    sscanf(args, "%d", &step);
  }
  cpu_exec(step);

  return 0;
}

static int cmd_info(char *args){
  if(strcmp(args, "r") == 0){
    isa_reg_display();
  }
  return 0;
}

#include "memory/vaddr.h"
static int cmd_x(char* args){
  char *N = strtok(NULL, " ");
  char *EXPR = strtok(NULL, " ");
  int len;
  vaddr_t address;

  sscanf(N, "%d", &len);
  sscanf(EXPR, "%x", &address);

  int i;
  printf("0x%x: ", address);
  for(i = 0;i<len;i++){
    printf("0x%x ", vaddr_read(address, 4));
    address += 4;
  }
  printf("\n");

  return 0;
}

static int cmd_p(char* args){
  if(args == NULL){
    printf("No parameters\n");
    return 0;
  }
  bool success = true;
  int num = expr(args, &success);
  if(success == false){
    printf("Wrong expression\n");
    return 0;
  }
  else{
    printf("0x%x %dD\n", num, num);
    return 0;
  }
}

static struct {
  const char *name;
  const char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  {"si", "Execute N instructions in once", cmd_si},
  {"info", "Print value of each register", cmd_info},
  {"x", "Scan and show N bytes in memory from the address EXPR", cmd_x},
  {"p", "Caculate the result of the given expression", cmd_p}

  /* TODO: Add more commands */

};

#define NR_CMD ARRLEN(cmd_table)

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

void sdb_set_batch_mode() {
  is_batch_mode = true;
}

void sdb_mainloop() {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL; ) {
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef CONFIG_DEVICE
    extern void sdl_clear_event_queue();
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}

void init_sdb() {
  /* Compile the regular expressions. */
  init_regex();

  /* Initialize the watchpoint pool. */
  init_wp_pool();
}
