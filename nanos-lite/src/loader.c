#include "fs.h"
#include <proc.h>
#include <memory.h>
#include <elf.h>
#include <stdint.h>

#if !defined(__ISA_AM_NATIVE__)
#include ISA_H
#endif

#ifdef __LP64__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
#endif

#if defined(__ISA_AM_NATIVE__)
# define EXPECT_TYPE EM_X86_64
#elif defined(__ISA_X86__)
# define EXPECT_TYPE EM_X86_64
#elif defined(__ISA_RISCV__)
# define EXPECT_TYPE EM_RISCV
#else
# error Unsupported ISA
#endif


static uintptr_t loader(PCB *pcb, const char *filename) {
  int fd = fs_open(filename, 0, 0);
  printf("fd is %d\n", fd);
  if (fd < 0) {
    panic("should not reach here");
  }
  Elf_Ehdr elf;
 
  assert(fs_read(fd, &elf, sizeof(elf)) == sizeof(elf));
  // check magic num
  assert(*(uint32_t *)elf.e_ident == 0x464c457f);
  
  Elf_Phdr phdr[elf.e_phnum];
  for (int i = 0; i < elf.e_phnum; i++) {
    uint32_t base = elf.e_phoff + i * elf.e_phentsize;
    fs_lseek(fd, base, 0);
    // read current phdr
    assert(fs_read(fd, &phdr[i], elf.e_phentsize) == elf.e_phentsize);
    // load
    if (phdr[i].p_type == PT_LOAD) {
      fs_lseek(fd, phdr[i].p_offset, 0);
      assert(fs_read(fd, (void*)phdr[i].p_vaddr, phdr[i].p_filesz) == phdr[i].p_filesz);
      memset((void*)phdr[i].p_vaddr + phdr[i].p_filesz, 0, phdr[i].p_memsz - phdr[i].p_filesz);
    }
  }
 
  assert(fs_close(fd) == 0);
  printf("load %s done,entry is %x\n", filename, elf.e_entry);
  
  return elf.e_entry;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %p", entry);
  ((void(*)())entry) ();
}

void context_uload(PCB *pcb, const char *filename, char *const argv[], char *const envp[]) {
    uintptr_t entry = loader(pcb, filename);

    // 计算argv和envp的长度
    int argc = 0;
    while (argv[argc] != NULL) {
        argc++;
    }

    int envc = 0;
    while (envp[envc] != NULL) {
        envc++;
    }

    // 计算所需的栈空间大小
    size_t stack_size = 0;
    stack_size += (argc + 1) * sizeof(uintptr_t);  // argv + NULL
    stack_size += (envc + 1) * sizeof(uintptr_t);  // envp + NULL
    stack_size += sizeof(int);                     // argc

    // 计算字符串的总长度
    for (int i = 0; i < argc; i++) {
        stack_size += strlen(argv[i]) + 1;
    }
    for (int i = 0; i < envc; i++) {
        stack_size += strlen(envp[i]) + 1;
    }

    // 调整栈指针到16字节对齐
    uintptr_t sp = (uintptr_t)new_page(8) - stack_size;
    sp = (sp - 16) & ~0xF;  // 16字节对齐

    // 填充字符串
    char *str_ptr = (char *)(sp + (argc + 1) * sizeof(uintptr_t) + (envc + 1) * sizeof(uintptr_t) + sizeof(int));
    char *str_tmp_ptr = str_ptr;
    for (int i = 0; i < argc; i++) {
        strcpy(str_tmp_ptr, argv[i]);
        str_tmp_ptr += strlen(argv[i]) + 1;
    }
    for (int i = 0; i < envc; i++) {
        strcpy(str_tmp_ptr, envp[i]);
        str_tmp_ptr += strlen(envp[i]) + 1;
    }

    uintptr_t *user_stack = (uintptr_t*)sp;
    *(int*)user_stack = argc;

    // 填充argv指针数组
    /* 1. 先定位到字符串区域的起始位置，以逐个获取字符串的地址；
       2. 然后定位到argv指针数组的起始位置；
       3. 将argv指针数组中的每个指针指向字符串区域；
    */
    uintptr_t *stack_argv = (uintptr_t*)(user_stack + 1);
    for(int i = 0; i < argc; i++) {
      stack_argv[i] = (uintptr_t)str_ptr;
      str_ptr += strlen(argv[i]) + 1;
    }
    stack_argv[argc] = 0;

    // 填充envp指针数组
    uintptr_t *stack_envp = (uintptr_t*)(stack_argv + argc + 1);
    for(int i = 0; i < envc; i++) {
      stack_envp[i] = (uintptr_t)str_ptr;
      str_ptr += strlen(envp[i]) + 1;
    }
    stack_envp[envc] = 0;
    

    // 创建上下文
    Context *uc = ucontext(&pcb->as, (Area){(void *)sp, (void *)pcb->stack}, (void *)entry);

    // 保存上下文指针到PCB
    pcb->cp = uc;

    // 设置栈指针到a0寄存器（RISC-V中a0传递第一个参数）
    pcb->cp->GPRx = (uintptr_t)user_stack;  // a0 = argc
}