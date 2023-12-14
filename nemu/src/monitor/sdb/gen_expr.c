// #include <stdio.h>
// #include <stdlib.h>
// #include <time.h>
// #include <assert.h>

// static int count =0;
// static char buf[60000];
// static char code_buf[65536];
// char *code_format = 
// "#include <stdio.h>\n"
// "int main(){"
// "  int result = %s; "
// "  printf(\"%%u\", result); "
// "  return 0; "
// "}";

// static int choose(unsigned int n){
//   return rand()%n;
// }

// static void gen_num(){
//   int num = choose(65536);
//   sprintf(buf+count, "%d", num);
//   while(buf[count]){
//     count++;
//   }
// }

// static void gen_rand_op(){
//   int num = choose(4);
//   switch(num){
//     case 0: 
//       sprintf(buf+count, "%c", '+');
//       break;
//     case 1:
//       sprintf(buf+count, "%c", '-');
//       break;
//     case 2:
//       sprintf(buf+count, "%c", '*');
//       break;
//     case 3:
//       sprintf(buf+count, "%c", '/');
//       break;
//   }
//   count++;
// }

// static void gen(char c){
//   sprintf(buf+count, "%c", c);
//   count++;
// }

// static void gen_rand_expr() {
//   int i = choose(3);
//   if(count>20) { i = 0; }

//   switch (i) {
//     case 0: gen_num(); break;
//     case 1: gen('('); gen_rand_expr(); gen(')'); break;
//     default: gen_rand_expr(); gen_rand_op(); gen_rand_expr(); break;
//   }
// }

// int main(int argc, char *argv[]){
//     int seed = time(0);
//     srand(seed);

//     int loop =1;
//     if(argc > 1){
//       sscanf(argv[1], "%d", &loop);
//     }
//     int i;
//     for(i=0;i<loop;i++){
//         // clear buf and count after last expression generation done
//         for (int j=0;j<(sizeof(buf)/sizeof(buf[0]));j++){
//           buf[i] = 0;
//         }
//         count = 0;

//         gen_rand_expr();
//         sprintf(code_buf, code_format, buf);
//         FILE *fp = fopen("/tmp/.code.c", "w");
//         assert(fp != NULL);
//         fputs(code_buf, fp);
//         fclose(fp);

//         int ret = system("gcc /tmp/.code.c -o /tmp/.expr");
//         if(ret) continue;
//         fp = popen("/tmp/.expr", "r");
//         assert(fp != NULL);
//         int result;
//         if(fscanf(fp, "%d\n", &result)==1)fclose(fp);
//         printf("%u %s\n", result, buf);
//     }
//     return 0;
// }