#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <parse_elf.h>

#define MAX_SYMBOLS 200
static FuncSymbolInfo_t func_symbol_info[MAX_SYMBOLS];
static size_t num_func_symbols = 0;
static bool initialized = false;
static int count = 0;

int is_current_section_string_section(const Elf32_Ehdr* elf_header, const Elf32_Shdr* section_headers, const char* shstrtab_data, const Elf32_Shdr* current_section) {
    const char* section_name = &shstrtab_data[current_section->sh_name];
    return current_section->sh_type == SHT_STRTAB && strcmp(section_name, ".strtab") == 0;
}

int find_symbol_table_and_strtab(const Elf32_Ehdr* elf_header, Elf32_Shdr* section_headers, const char* shstrtab_data, ElfSymbolInfo* out_symbol_info) {
    Elf32_Shdr* sym_tab_shdr = NULL;
    Elf32_Shdr* str_tab_shdr = NULL;

    for (size_t i = 0; i < elf_header->e_shnum; ++i) {
        Elf32_Shdr* current_section = &section_headers[i];

        if (is_current_section_string_section(elf_header, section_headers, shstrtab_data, current_section)) {
            str_tab_shdr = current_section;
        } else if (current_section->sh_type == SHT_SYMTAB) {
            sym_tab_shdr = current_section;
        }

        if (sym_tab_shdr && str_tab_shdr) {
            break;
        }
    }

    if (!sym_tab_shdr || !str_tab_shdr) {
        return 1;  // Failed to find symbol table or string table
    }

    out_symbol_info->symbols = (Elf32_Sym*)((char*)elf_header + sym_tab_shdr->sh_offset);
    out_symbol_info->strTabData = (char*)elf_header + str_tab_shdr->sh_offset;
    out_symbol_info->numSymbols = sym_tab_shdr->sh_size / sizeof(Elf32_Sym);

    return 0;  // Success
}

static void store_func_symbol_info(const ElfSymbolInfo* symbol_info) {
    //int num_func_symbols = 0;
    for (size_t i = 0; i < symbol_info->numSymbols; ++i) {
        const Elf32_Sym* symbol = &symbol_info->symbols[i];
        const char* name = symbol_info->strTabData + symbol->st_name;

        // printf("Symbol %zu: Name=%s, Value=0x%08x, Size=%lu, Type=%u, Bind=%u, Visibility=%u\n",
        //        i, name, symbol->st_value, symbol->st_size, symbol->st_info & 0xF, symbol->st_info >> 4, symbol->st_other & 0x3);

        // Additional processing for function symbols
        if (ELF32_ST_TYPE(symbol->st_info) == STT_FUNC) {
            func_symbol_info[num_func_symbols].name = symbol->st_name;
            func_symbol_info[num_func_symbols].address = symbol->st_value;
            func_symbol_info[num_func_symbols].size = symbol->st_size;
            num_func_symbols++;
        }
    }
    initialized = true;
    printf("The num of func symbols is %d\n", num_func_symbols);
}

FuncSymbolInfo_t get_func_symbol_info(uint32_t pc, uint32_t func_address, const ElfSymbolInfo* symbol_info, uint32_t mode) {
    for(size_t i = 0; i < num_func_symbols; ++i) {
        //printf("The func address is %x\n", func_address);
        //printf("The symbol start is %x\n", func_symbol_info[i].address);
        //printf("The symbol end is %x\n", func_symbol_info[i].address + func_symbol_info[i].size);
        if (func_address >= func_symbol_info[i].address && \
            func_address < func_symbol_info[i].address + func_symbol_info[i].size) {
            //printf("The symbol %s address is %x\n", symbol_info->strTabData + func_symbol_info[i].name, func_symbol_info[i].address);
            printf("%x", pc);
            if(mode == 0) {
                printf("%*s", count, "");
                printf(" call[%s@%x]\n", symbol_info->strTabData + func_symbol_info[i].name, func_symbol_info[i].address);
                count++;
            }
            else{
                count--;
                printf("%*s", count, "");
                printf(" ret [%s]\n", symbol_info->strTabData + func_symbol_info[i].name, func_symbol_info[i].address);
            }
            
            return func_symbol_info[i];
        }
    }
    return (FuncSymbolInfo_t){0};
}

FuncSymbolInfo_t get_func_name_with_pc(uint32_t current_pc, uint32_t func_address, char* elf_path, uint32_t mode) {
    // if (argc < 2) {
    //     printf("Usage: %s <ELF file>\n", argv[0]);
    //     return 1;
    // }

    // const char* elf_path = argv[1];
    FILE* elf_file = fopen(elf_path, "rb");
    if (elf_file == NULL) {
        perror("Failed to open ELF file");
        //return 1;
    }

    fseek(elf_file, 0, SEEK_END);
    long file_size = ftell(elf_file);
    fseek(elf_file, 0, SEEK_SET);

    void* mapped_file = mmap(NULL, file_size, PROT_READ, MAP_PRIVATE, fileno(elf_file), 0);
    if (mapped_file == MAP_FAILED) {
        perror("Failed to map ELF file");
        fclose(elf_file);
        //return 1;
    }

    Elf32_Ehdr* elf_header = (Elf32_Ehdr*)mapped_file;
    Elf32_Shdr* section_headers = (Elf32_Shdr*)((char*)mapped_file + elf_header->e_shoff);
    char* shstrtab_data = (char*)mapped_file + section_headers[elf_header->e_shstrndx].sh_offset;

    ElfSymbolInfo symbol_info;
    if (find_symbol_table_and_strtab(elf_header, section_headers, shstrtab_data, &symbol_info)) {
        fprintf(stderr, "Failed to find symbol table or string table (.strtab)\n");
        munmap(mapped_file, file_size);
        fclose(elf_file);
        //return 1;
    }

    if(initialized == false){
        store_func_symbol_info(&symbol_info);
    }
    // store_func_symbol_info(&symbol_info);
    
    
    FuncSymbolInfo_t func_symbol_info = get_func_symbol_info(current_pc, func_address, &symbol_info, mode);

    //print_symbols(&symbol_info);

    munmap(mapped_file, file_size);
    fclose(elf_file);

    return func_symbol_info;
}