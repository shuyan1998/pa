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
  if (fd < 0) {
    panic("should not reach here");
  }
  Elf_Ehdr elf;
 
  assert(fs_read(fd, &elf, sizeof(elf)) == sizeof(elf));
  // 检查魔数
  assert(*(uint32_t *)elf.e_ident == 0x464c457f);
  
  Elf32_Phdr phdr[elf.e_phnum];
  for (int i = 0; i < elf.e_phnum; i++) {
    uint32_t base = elf.e_phoff + i * elf.e_phentsize;
 
    fs_lseek(fd, base, 0);
    assert(fs_read(fd, &phdr[i], elf.e_phentsize) == elf.e_phentsize);
    
    // 需要装载的段
    if (phdr[i].p_type == PT_LOAD) {
 
      // char * buf_malloc = (char *)malloc(phdr.p_filesz);
      
      fs_lseek(fd, phdr[i].p_offset, 0);
      assert(fs_read(fd, (void*)phdr[i].p_vaddr, phdr[i].p_filesz) == phdr[i].p_filesz);
      
      // memcpy((void*)phdr.p_vaddr, buf_malloc, phdr.p_filesz);
      memset((void*)phdr[i].p_vaddr + phdr[i].p_filesz, 0, phdr[i].p_memsz - phdr[i].p_filesz);
      
      // free(buf_malloc);
    }
  }
 
  assert(fs_close(fd) == 0);
  
  return elf.e_entry;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %p", entry);
  ((void(*)())entry) ();
}

