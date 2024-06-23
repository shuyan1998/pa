#ifndef    __PARSE_ELF_H__
#define    __PARSE_ELF_H__

#include <elf.h>
#include <stdint.h>

typedef struct {
    Elf32_Sym* symbols;
    char* strTabData;
    uint32_t numSymbols;
} ElfSymbolInfo;

typedef struct {
    Elf32_Word name;
    Elf32_Addr address;
    Elf32_Xword size;
} FuncSymbolInfo_t;

typedef enum {
    CALL_MODE,
    RETURN_MODE
}Mode;

int is_current_section_string_section(const Elf32_Ehdr* elf_header, const Elf32_Shdr* section_headers, const char* shstrtab_data, const Elf32_Shdr* current_section);
int find_symbol_table_and_strtab(const Elf32_Ehdr* elf_header, Elf32_Shdr* section_headers, const char* shstrtab_data, ElfSymbolInfo* out_symbol_info);
static void store_func_symbol_info(const ElfSymbolInfo* symbol_info);
static FuncSymbolInfo_t get_func_symbol_info(uint32_t pc, uint32_t func_address, const ElfSymbolInfo* symbol_info, uint32_t mode);
FuncSymbolInfo_t get_func_name_with_pc(uint32_t current_pc, uint32_t func_address, char* elf_path, uint32_t mode);

#endif  