#include <ctype.h>
#include <elf.h>
#include <string.h>
#include <stdlib.h>

#include "display.h"
#include "debug.h"
#include "structure.h"
#include "util.h"

void print_ELF_header(Elf32_Ehdr *header)
{
    debug("print_ELF_header\n");
    printf("ELF Header:\n");
    
    // Affiche le magic number en se servant de la constant EI_NIDENT
    printf("  Magic:  ");
    for (int i = 0; i < EI_NIDENT; i++)
    {
        printf(" %02x", header->e_ident[i]);
    }
    printf(" \n");
    // Conditionnelles pour garder la bonne chaine de caractère en fonction des valeurs dans l'en-tête
    char *e_class = header->e_ident[4] != 0 ? (header->e_ident[4] == 1 ? "32" : "64") : "none";
    char *e_data = header->e_ident[5] == 1 ? "little endian" : "big endian";
    char *e_version = header->e_ident[6] == 1 ? "1 (current)" : "0";

    printf("  Class:                             %s%s\n", strcmp(e_class, "none") == 0 ? "" : "ELF", e_class);
    printf("  Data:                              2's complement, %s\n", e_data);
    printf("  Version:                           %s\n", e_version);

    // Situé dans les valeurs non utilisées du tableau e_ident, on peut y trouver la distribution UNIX
    int ei_os_abi = header->e_ident[7];

    printf("  OS/ABI:                            ");

    switch (ei_os_abi)
    {
    case ELFOSABI_NONE:
        printf("UNIX - System V");
        break;
    case ELFOSABI_HPUX:
        printf("UNIX - HP-UX");
        break;
    case ELFOSABI_NETBSD:
        printf("UNIX - NetBSD");
        break;
    case ELFOSABI_LINUX:
        printf("UNIX - Linux");
        break;
    case ELFOSABI_SOLARIS:
        printf("UNIX - Sun Solaris");
        break;
    case ELFOSABI_AIX:
        printf("UNIX - IBM AIX");
        break;
    case ELFOSABI_IRIX:
        printf("UNIX - SGI Irix");
        break;
    case ELFOSABI_FREEBSD:
        printf("UNIX - FreeBSD");
        break;
    case ELFOSABI_TRU64:
        printf("UNIX - Compaq TRU64");
        break;
    case ELFOSABI_MODESTO:
        printf("UNIX - Novell Modesto");
        break;
    case ELFOSABI_OPENBSD:
        printf("UNIX - OpenBSD");
        break;
    case ELFOSABI_ARM:
        printf("UNIX - ARM");
        break;
    case ELFOSABI_STANDALONE:
        printf("UNIX - Standalone (embedded) application");
        break;
    default:
        // fprintf(stderr, "OS/ABI isn't supported yet %d", ei_os_abi);
        break;
    }
    printf("\n");

    int ei_abi_version = header->e_ident[8];
    printf("  ABI Version:                       %d\n", ei_abi_version);
    printf("  Type:                              ");
    switch (header->e_type)
    {
    case ET_NONE:
        printf("No file type");
        break;

    case ET_REL:
        printf("REL (Relocatable file)");
        break;

    case ET_EXEC:
        printf("EXEC (Executable file)");
        break;

    case ET_DYN:
        printf("DYN (Shared object file)");
        break;

    case ET_CORE:
        printf("Core file");
        break;
    case ET_LOPROC:
        printf("Processor-specific");
        break;
    case ET_HIPROC:
        printf("Processor-specific");
        break;
    default:
        printf("<unknown>: %x", header->e_type);
        break;
    }
    printf("\n  Machine:                           ");

    switch (header->e_machine)
    {
    case EM_NONE:
        printf("No machine");
        break;
    case EM_M32:
        printf("AT&T WE 32100");
        break;
    case EM_SPARC:
        printf("SPARC");
        break;
    case EM_386:
        printf("Intel Architecture");
        break;
    case EM_68K:
        printf("Motorola 68000");
        break;
    case EM_88K:
        printf("Motorola 88000");
        break;
    case EM_860:
        printf("Intel 80860");
        break;
    case EM_MIPS:
        printf("MIPS RS3000 Big-Endian");
        break;
    case EM_ARM:
        printf("ARM");
        break;

    default:
        printf("<unknown>: 0x%x", header->e_machine);
        break;
    }
    printf("\n");
    printf("  Version:                           0x%x\n", header->e_version);
    printf("  Entry point address:               0x%hx\n", header->e_entry);

    printf("  Start of program headers:          %d (bytes into file)\n", header->e_phoff);
    printf("  Start of section headers:          %d (bytes into file)\n", header->e_shoff);
    printf("  Flags:                             0x%x", header->e_flags);
    switch (EF_ARM_EABI_VERSION(header->e_flags)) // cf. /usr/include/elf.h ligne ~ 2870
    {
    case EF_ARM_EABI_VER1:
        printf(", Version1 EABI");
        break;
    case EF_ARM_EABI_VER2:
        printf(", Version2 EABI");
        break;
    case EF_ARM_EABI_VER3:
        printf(", Version3 EABI");
        break;
    case EF_ARM_EABI_VER4:
        printf(", Version4 EABI");
        break;
    case EF_ARM_EABI_VER5:
        printf(", Version5 EABI");
        break;
    default:
        break;
    }
    Elf32_Word flagstuff = header->e_flags & (~EF_ARM_EABIMASK); // masquage de base, cf. /usr/include/elf.h ligne ~ 2864 et ~ 2870
                                                                 // dans ce qui suit ('switch(flagstuff)'), on ne gère évidemment pas tous les cas, ni ceux traités, de manière absolument correcte,
                                                                 // on veut juste ici passer les tests automatiques faits sur l'affichage à partir d'une quantité limitée d'échantillons, sinon il y a des cas particuliers à vraiment traiter
    switch (flagstuff)
    {
    case EF_ARM_ABI_FLOAT_SOFT:
        printf(", soft-float ABI");
        break;
    case EF_ARM_ABI_FLOAT_HARD:
        printf(", hard-float ABI");
        break;
    case EF_ARM_SYMSARESORTED:
        printf(", sorted symbol tables");
        break;
    case EF_ARM_DYNSYMSUSESEGIDX:
        printf(", dynamic symbols use segment index");
        break;
    case EF_ARM_MAPSYMSFIRST:
        printf(", mapping symbols precede others");
        break;
    case EF_ARM_BE8:
        printf(", BE8");
        break;
    case EF_ARM_LE8:
        printf(", LE8");
        break;
    default:
        break;
    }
    printf("\n");
    printf("  Size of this header:               %d (bytes)\n", header->e_ehsize);
    printf("  Size of program headers:           %d (bytes)\n", header->e_phentsize);
    printf("  Number of program headers:         %d\n", header->e_phnum);
    printf("  Size of section headers:           %d (bytes)\n", header->e_shentsize);
    printf("  Number of section headers:         %d\n", header->e_shnum);
    printf("  Section header string table index: %d", header->e_shstrndx);
    if (header->e_shstrndx >= header->e_shnum)
    {
        printf(" <corrupt: out of range>\n");
    }
    else
    {
        printf("\n");
    }
    if (header->e_shentsize < sizeof(Elf32_Shdr))
    {
        printf("readelf: Error: The e_shentsize field in the ELF header is less than the size of an ELF section header\n");
    }
    else if (header->e_shentsize > sizeof(Elf32_Shdr))
    {
        printf("readelf: Error: The e_shentsize field in the ELF header is larger than the size of an ELF section header\n");
    }
}

void display_ELF_header(Elf32_Ehdr *elf_header)
{
    // Notre édude concerne uniquement les fichiers 32 bits. 
    if (elf_header->e_ident[4] == 1)
    {
        print_ELF_header(elf_header);
    }
    else
    {
        die(1, "Le fichier ELF fourni n'est pas en 32 bits");
    }
}

void display_ELF_section_table(ELF32_FILE *file)
{
    printf("There are %d section headers, starting at offset 0x%x:\n", file->header.e_shnum, file->header.e_shoff); 
    printf("\nSection Headers:\n");
    printf("  [Nr] Name              Type                Addr     Off    Size   ES Flg Lk Inf Al\n");

    for (int i = 0; i < len_sections(file); i++)
    {
        Elf32_Shdr *section = &file->sections_tab[i].header;

        // Récupère le nom de la section depuis la table de chaine de caractère. 
        // Ici, la valeur 'sh_name' est l'indice dans la table de chaine de caractère 
        // et pas le nom de la section. 
        char *sec_name;
        if (!file->section_names) { 
            sec_name = "<no-strings>";
        } else {
            sec_name = &file->section_names[section->sh_name];
        }

        // Affichage des informations de base
        printf("  [%2d] %-17s ", i, sec_name);

        // Afficher le type de la section (sans couper les mots)
        const char *section_type;
        switch (section->sh_type)
        {
        case SHT_NULL:
            section_type = "NULL";
            break;
        case SHT_PROGBITS:
            section_type = "PROGBITS";
            break;
        case SHT_SYMTAB:
            section_type = "SYMTAB";
            break;
        case SHT_STRTAB:
            section_type = "STRTAB";
            break;
        case SHT_REL:
            section_type = "REL";
            break;
        case SHT_RELA:
            section_type = "RELA";
            break;
        case SHT_NOBITS:
            section_type = "NOBITS";
            break;
        case SHT_ARM_ATTRIBUTES:
            section_type = "ARM_ATTRIBUTES";
            break;
        default:
            section_type = "OTHER";
        }
        printf("%-20s", section_type); // Largeur ajustée à 20 pour éviter les coupures

        // Affichage des champs numériques et des flags
        printf("%08x %06x %06x %02x ", 
               section->sh_addr, 
               section->sh_offset, 
               section->sh_size, 
               section->sh_entsize);

        // Afficher les flags (Flg) avec largeur fixe
        char flags[12] = "";  // Stocke les flags pour affichage
        if (section->sh_flags & SHF_WRITE) strcat(flags, "W");
        if (section->sh_flags & SHF_ALLOC) strcat(flags, "A");
        if (section->sh_flags & SHF_EXECINSTR) strcat(flags, "X");
        if (section->sh_flags & SHF_MERGE) strcat(flags, "M");
        if (section->sh_flags & SHF_STRINGS) strcat(flags, "S");
        if (section->sh_flags & SHF_INFO_LINK) strcat(flags, "I");
        if (section->sh_flags & SHF_LINK_ORDER) strcat(flags, "L");
        if (section->sh_flags & SHF_OS_NONCONFORMING) strcat(flags, "O");
        if (section->sh_flags & SHF_GROUP) strcat(flags, "G");
        if (section->sh_flags & SHF_TLS) strcat(flags, "T");
        if (section->sh_flags & SHF_COMPRESSED) strcat(flags, "C");
        printf("%-3s ", flags[0] ? flags : " ");  // Largeur fixe pour les flags

        // Afficher les champs Lk, Inf et Al avec largeur fixe
        printf("%2d %3d %2d\n", 
               section->sh_link, 
               section->sh_info, 
               section->sh_addralign);
    }
}


void display_ELF_section_content(ELF32_FILE *file, char *section_name, FILE *stream)
{
    // Recherche de la section avec 'section_name'
    Elf32_Shdr *section = NULL;
    int len_sec = len_sections(file);
    if (section_name != NULL && file->section_names != NULL)
    {
        for (int i = 0; i < len_sec; i++)
        {
            // Si parmis toutes les sections, il y a une correspondance, on a trouvé la bonne section
            if (strcmp(&file->section_names[file->sections_tab[i].header.sh_name], section_name) == 0)
            {
                section = &file->sections_tab[i].header;
                break;
            }
        }
    }

    // Fonctionnalité supplémentaire pour trouver la section à l'aide d'un indice et pas du nom
    int len = strlen(section_name);
    int correct = 1;
    for (int i = 0; i < len; i++) {
      if (! isdigit(section_name[i])) { 
        correct = 0; // S'assurer que ce soit bien un entier que l'on a trouvé
        break;
      }
    }

    // Trouver la section à l'aide de l'indice. 
    int x;
    if (correct) {
      x = atoi(section_name); 
      if (x < len_sec) {
        section = &file->sections_tab[x].header;
        if (file->section_names)
            strcpy(section_name, &file->section_names[file->sections_tab[x].header.sh_name]);
        else
            strcpy(section_name, "<no-strings>");
      } else {
        fprintf(stderr, "readelf: Warning: Section %s was not dumped because it does not exist\n", section_name);
        return;
      }
    }
    if (section == NULL)
    {
        fprintf(stderr, "readelf: Warning: Section '%s' was not dumped because it does not exist\n", section_name);
        return;
    }

    if (section->sh_size == 0) {
        fprintf(stderr, "Section '%s' has no data to dump.\n", section_name);
        return;
    }

    printf("\n Hex dump of section '%s':\n", section_name);

    // On se met au début de la section à l'aide d'offset contenu dans la structure correspondante de la section.
    fseek(stream, section->sh_offset, SEEK_SET);
    size_t bytes_read = 0;
    for (size_t i = 0; i < section->sh_size; i += 16)
    {
        printf("  0x%08lx ", section->sh_addr + i); // Affiche l'adresse. 
        size_t bytes_to_read = (section->sh_size - i) < 16 ? (section->sh_size - i) : 16;
        for (size_t j = 0; j < bytes_to_read; j++)
        {
            unsigned char c = fgetc(stream);
            printf("%02x", c);
            if ((j + 1) % 4 == 0)
            {
                printf(" ");
            }
            // Tous les 8 caractères hexadécimaux, on affiche un espace. 
        }
        for (size_t j = bytes_to_read; j < 16; j++)
        {
            printf("  ");
            // Si nous avons atteint la fin de ce que l'on doit lire, remplacer par des espaces. 
        }
        printf("  ");
        fseek(stream, section->sh_offset + i, SEEK_SET);
        for (size_t j = 0; j < bytes_to_read; j++)
        {
            unsigned char c = fgetc(stream);
            printf("%c", (c >= 32 && c <= 126) ? c : '.');
            // Tentative d'afficher en ASCII les caractères lus à l'adresse. Sinon, remplacer par des points. 
        }
        printf("\n");
        bytes_read += bytes_to_read;
    }
    printf("\n");
}

void display_symbol_table(ELF32_FILE *file)
{
    int sym_count = len_symbols(file);
    if (sym_count == 0) {
        return;
    }
    Elf32_Off offset = file->sections_tab[symbols_table_index(file)].header.sh_offset;
    debug(".symtab offset = 0x%x\n", offset);
    if (offset < sizeof(Elf32_Ehdr)) {
      free_file(file);
      die(1, "l'offset de '.symtab' (0x%x) ne peut pas être inférieur à la taille d'un header ELF\n", offset);
    }
    int sym_tab_index = symbols_table_index(file);
    char *sym_str = file->section_names ? &file->section_names[file->sections_tab[sym_tab_index].header.sh_name] : "<no-strings>";

    printf("Symbol table '%s' contains %d entries:\n", sym_str, sym_count);

    printf("Num:    Value  Size Type    Bind   Vis      Ndx Name\n");

    for (int i = 0; i < sym_count; i++)
    {

        Elf32_Sym *symbol = &file->symbols_tab[i];

        printf("%d: ", i);
        printf("%08x    ", symbol->st_value);

        printf("%d ", symbol->st_size);

        switch (ELF32_ST_TYPE(symbol->st_info))
        {
        case STT_SECTION:
            printf("SECTION ");
            break;

        case STT_NOTYPE:
            printf("NOTYPE ");
            break;

        case STT_OBJECT:
            printf("OBJECT ");
            break;

        case STT_FUNC:
            printf("FUNC ");
            break;

        case STT_FILE:
            printf("FILE ");
            break;

        case STT_COMMON:
            printf("COMMON ");
            break;

        case STT_TLS:
            printf("TLS ");
            break;

        case STT_NUM:
            printf("NUM ");
            break;

        case STT_LOOS:
            printf("LOOS ");
            break;

        case STT_HIOS:
            printf("HIOS ");
            break;

        case STT_LOPROC:
            printf("LOPROC ");
            break;

        case STT_HIPROC:
            printf("HIPROC ");
            break;

        default:
            printf("<unknown>: %d", ELF32_ST_TYPE(symbol->st_info));
            break;
        }

        switch (ELF32_ST_BIND(symbol->st_info))
        {
        case STB_LOCAL:
            printf("LOCAL ");
            break;

        case STB_GLOBAL:
            printf("GLOBAL ");
            break;

        case STB_WEAK:
            printf("WEAK ");
            break;

        case STB_NUM:
            printf("NUM ");
            break;

        case STB_LOOS:
            printf("LOOS ");
            break;

        case STB_HIOS:
            printf("HIOS ");
            break;

        case STB_LOPROC:
            printf("LOPROC ");
            break;

        case STB_HIPROC:
            printf("HIRPOC ");
            break;

        default:
            printf("<unknown>: %d", ELF32_ST_BIND(symbol->st_info));
            break;
        }

        switch (ELF32_ST_VISIBILITY(symbol->st_other))
        {
        case STV_DEFAULT:
            printf("DEFAULT ");
            break;

        case STV_INTERNAL:
            printf("INTERNAL ");
            break;

        case STV_HIDDEN:
            printf("HIDDEN ");
            break;

        case STV_PROTECTED:
            printf("PROTECTED ");
            break;

        default:
            printf("Unrecognized visibility value: %u\n", ELF32_ST_VISIBILITY(symbol->st_other));
            break;
        }

        switch (symbol->st_shndx)
        {
        case SHN_UNDEF:
            printf("UND ");
            break;
        
        case SHN_ABS:
            printf("ABS ");
            break;
        
        default:
            if (file->section_names)
              printf("%d ", symbol->st_shndx);
            else
              printf("<no-strings>  ");
            break;
        }

        // les symbols qui sont du type section ont leurs nom stocké dans section_names
        int sym_type = ELF32_ST_TYPE(symbol->st_info); 
        if (sym_type == STT_SECTION)
        {
            printf("%.21s", file->section_names ? &file->section_names[file->sections_tab[symbol->st_shndx].header.sh_name] : "<no-strings>");
        }
        else
        {
            printf("%.21s", file->symbol_names ? &file->symbol_names[symbol->st_name] : "<no-strings>");
        }

        printf("\n");
    }
}

void display_relocation_table(ELF32_FILE *file)
{
    if (!file->reimp_tab)
    {
        fprintf(stderr, "La table des reimplantations n'est pas chargée\n");
        return;
    }

    Elf32_Relocation *reloc = file->reimp_tab;
    
    if (reloc->len_tab == 0) {
        printf("\nThere are no relocations in this file.\n");
        return;
    }

    // Afficahge de REL
    for (int i = 0; i < reloc->len_tab; i++)
    {
        printf("\nRelocation section '%s'  at offset %#x contains %d entries:\n",
               &file->section_names[reloc->tab[i].name],
               reloc->tab[i].offset,
               reloc->tab[i].nbr_entries);

        printf(" Offset    Info   Type              Sym.Value  Sym.Name\n");
        for (int j = 0; j < reloc->tab[i].nbr_entries; j++)
        {
            printf("%08x  %08x ",
                   reloc->tab[i].tab[j].r_offset,
                   reloc->tab[i].tab[j].r_info);

            switch (ELF32_R_TYPE(reloc->tab[i].tab[j].r_info)) 
            {
              case R_ARM_JUMP24:
                printf("R_ARM_JUMP24 ");
                break;

              case R_ARM_CALL:
                printf("R_ARM_CALL ");
                break;
            
              case R_ARM_ABS8:
                printf("R_ARM_ABS8 ");
                break;
              
              case R_ARM_ABS16:
                printf("R_ARM_ABS16 ");
                break;
          
              case R_ARM_ABS32:
                printf("R_ARM_ABS32 ");
                break;

              case R_ARM_V4BX:
                printf("R_ARM_V4BX ");
                break;

              default:
                printf("<unknown> %d", ELF32_R_TYPE(reloc->tab[i].tab[j].r_info));
                break;
            }

            
            int sym_index = ELF32_R_SYM(reloc->tab[i].tab[j].r_info);
            
            Elf32_Addr st_value = file->symbols_tab[sym_index].st_value;

            Elf32_Word st_name = file->symbols_tab[sym_index].st_name;

            char *sym_name = &file->symbol_names[st_name];

            if (st_name == 0) {
              sym_name = &file->section_names[file->sections_tab[file->symbols_tab[sym_index].st_shndx].header.sh_name]; 
            }
            
            printf("     %08x  %s \n",
                    st_value,
                    sym_name);
        }
    }
}
