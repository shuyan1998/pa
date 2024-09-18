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
  // get elf header
  Elf_Ehdr ehdr;
  ramdisk_read(&ehdr, 0, sizeof(Elf_Ehdr));
  printf("Magic num is %x\n", ehdr.e_ident);

  // check magic number
  // assert(*(uint32_t *)ehdr.e_ident == 0x7f454c46);

  // check architectuture
  assert(ehdr.e_machine == EXPECT_TYPE);

  //get program header
  Elf32_Phdr phdr[ehdr.e_phnum];
  ramdisk_read(phdr, ehdr.e_ehsize, ehdr.e_phnum * sizeof(Elf32_Phdr));
  for(int i = 0;i < ehdr.e_phnum;i++) {
    if(phdr[i].p_type == PT_LOAD) {
       ramdisk_read((void*)phdr[i].p_vaddr, phdr[i].p_offset, phdr[i].p_memsz);
       memset((void*)(phdr[i].p_vaddr + phdr[i].p_filesz), 0, phdr[i].p_memsz - phdr[i].p_filesz);
     }
  }

  return ehdr.e_entry;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %p", entry);
  ((void(*)())entry) ();
}

