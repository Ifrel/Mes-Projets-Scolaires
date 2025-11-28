#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#include <elf.h>
#include "structure.h"

void print_ELF_header(Elf32_Ehdr *header);

void display_ELF_header(Elf32_Ehdr *elf_header);
void display_ELF_section_table(ELF32_FILE *file);
void display_ELF_section_content(ELF32_FILE *file, char *section_name, FILE *stream);
void display_symbol_table(ELF32_FILE *file);

void display_relocation_table(ELF32_FILE *file);
#endif /* __NOTRE_STRUCTURE__ */
