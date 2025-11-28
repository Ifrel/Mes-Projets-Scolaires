#include "structure.h"
#include "io_fun.h"
#include "status.h"
#include "util.h"
#include "debug.h"
#include "csapp.h"

#include <elf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

ERR_CODE read_file(FILE *stream, ELF32_FILE *file)
{
    fseek(stream, 0, SEEK_END);
    if ((file->taille_fichier = ftell(stream)) == 0)
    {
        return ERR_READ;
    }
    rewind(stream);

    init_file(file);

    ERR_CODE ret1;
    if ((ret1 = read_elf_header(file, stream)) != OK ||
        read_section_tab(file, stream) != OK ||
        read_symbol_tab(file, stream) != OK ||
        read_reimp_tab(file, stream) != OK)
    {
        if (ret1 == ERR_MAGIC_BYTES)
        {
            return ERR_MAGIC_BYTES;
        }
        return ERR_READ;
    }

    return OK;
}

void init_file(ELF32_FILE *file)
{
    file->sections_tab = NULL;
    file->section_names = NULL;
    file->symbol_names = NULL;
    file->symbols_tab = NULL;
    file->reimp_tab = NULL;

}

void free_file(ELF32_FILE *file)
{
    if (file->sections_tab)
    {
        int len_sec = len_sections(file);
        for (int i = 0; i < len_sec; i++)
        {
            if (file->sections_tab[i].content)
            {
                free(file->sections_tab[i].content);
            }
        }
        free(file->sections_tab);
    }
    if (file->section_names)
        free(file->section_names);
    if (file->symbol_names)
        free(file->symbol_names);
    if (file->symbols_tab)
        free(file->symbols_tab);
    if (file->reimp_tab)
        free(file->reimp_tab);
}

Elf32_Half len_sections(ELF32_FILE *file)
{
    return file->header.e_shnum;
}

Elf32_Off offset_sections(ELF32_FILE *file)
{
    return file->header.e_shoff;
}

/********************************************************************
 *                       FONCTIONS DE LECTURES                       *
 *                                                                   *
 *   Description: Sont utilisées pour lire les differents parties    *
 *               d'un ficher ELF                                     *
 *********************************************************************/
ERR_CODE read_elf_header(ELF32_FILE *file, FILE *stream)
{

    Elf32_Ehdr *header = &file->header;

    // Lecture des identifiants ELF
    if (fread(header->e_ident, sizeof(header->e_ident), 1, stream) != 1)
    {
        /*perror("Erreur lors de la lecture des identifiants ELF");*/
        return ERR_READ;
    }
    if (header->e_ident[0] != 0x7f)
    {
        goto bad_id;
    }
    if (header->e_ident[1] != 0x45)
    {
        goto bad_id;
    }
    if (header->e_ident[2] != 0x4c)
    {
        goto bad_id;
    }
    if (header->e_ident[3] != 0x46)
    {
        goto bad_id;
    }
    if ((header->e_ident[4] < 0) || (header->e_ident[4] > 2))
    {
        goto bad_id;
    }
    if ((header->e_ident[5] < 0) || (header->e_ident[5] > 2))
    {
        goto bad_id;
    }
    if ((header->e_ident[6] < 0) || (header->e_ident[6] > 2))
    {
        goto bad_id;
    }
    if (header->e_ident[6] > 2)
    {
        goto bad_id;
    }
    for (int i = 8; i < EI_NIDENT; i++)
    {
        if (header->e_ident[i] != 0)
        {
            goto bad_id;
        }
    }

    // Lecture des autres champs de l'en-tête ELF
    if (get_u16(stream, &header->e_type) ||
        get_u16(stream, &header->e_machine) ||
        get_u32(stream, &header->e_version) ||
        get_u32(stream, &header->e_entry) ||
        get_u32(stream, &header->e_phoff) ||
        get_u32(stream, &header->e_shoff) ||
        get_u32(stream, &header->e_flags) ||
        get_u16(stream, &header->e_ehsize) ||
        get_u16(stream, &header->e_phentsize) ||
        get_u16(stream, &header->e_phnum) ||
        get_u16(stream, &header->e_shentsize) ||
        get_u16(stream, &header->e_shnum) ||
        get_u16(stream, &header->e_shstrndx))
    {
        /*perror("Erreur lors de la lecture d'un champ de l'en-tête ELF");*/
        return ERR_READ;
    }

    int total_sections_bytes = header->e_shnum * sizeof(Elf32_Shdr);
    if (total_sections_bytes > file->taille_fichier || header->e_shoff + total_sections_bytes > file->taille_fichier)
    {
        free_file(file);
        die(1, "readelf: Error: Reading %d bytes extends past end of file for section headers\n", total_sections_bytes);
    }

    return OK;
bad_id:
    fprintf(stderr, "readelf: Error: Not an ELF file - it has the wrong magic bytes at the start\n");
    return ERR_MAGIC_BYTES;
}

ERR_CODE read_section_tab(ELF32_FILE *file, FILE *stream)
{

    int len_sec = len_sections(file);
    file->sections_tab = malloc(len_sec * sizeof(Elf32_ShdrFull));
    Elf32_ShdrFull *sections = file->sections_tab;
    if (!sections)
    {
        error("Erreur d'allocation mémoire pour les en-têtes de section");
        return ERR_ALLOC;
    }

    file->old_len_sections = len_sec;
    // Deplacement du curseur
    fseek(stream, offset_sections(file), SEEK_SET);

    // Lecture des en-têtes de section
    for (int i = 0; i < len_sec; i++)
    {
        if (get_u32(stream, &sections[i].header.sh_name) ||
            get_u32(stream, &sections[i].header.sh_type) ||
            get_u32(stream, &sections[i].header.sh_flags) ||
            get_u32(stream, &sections[i].header.sh_addr) ||
            get_u32(stream, &sections[i].header.sh_offset) ||
            get_u32(stream, &sections[i].header.sh_size) ||
            get_u32(stream, &sections[i].header.sh_link) ||
            get_u32(stream, &sections[i].header.sh_info) ||
            get_u32(stream, &sections[i].header.sh_addralign) ||
            get_u32(stream, &sections[i].header.sh_entsize))
        {
            error("Erreur lors de la lecture d'un champ de de la table des sections\n");
            free(sections);
            return ERR_READ;
        }
    }
    for (int i = 0; i < len_sec; i++)
    {

        fseek(stream, sections[i].header.sh_offset, SEEK_SET);

        sections[i].content = (uint32_t *)malloc(sections[i].header.sh_size);

        if (sections[i].content == NULL)
        {
            free(sections);
            return ERR_ALLOC;
        }

        fread(sections[i].content, sections[i].header.sh_size, 1, stream);
    }

    // Lire la section contenant les noms des sections (shstrtab)
    Elf32_Shdr shstrtab = sections[file->header.e_shstrndx].header;
    if (shstrtab.sh_size == 0)
    {
        return OK;
    }

    file->section_names = malloc(shstrtab.sh_size);
    if (!file->section_names)
    {
        error("Erreur allocation mémoire pour les noms des sections");
        free(sections);
        return ERR_ALLOC;
    }

    fseek(stream, shstrtab.sh_offset, SEEK_SET);
    fread(file->section_names, shstrtab.sh_size, 1, stream);

    return OK;
}

int symbols_table_index(ELF32_FILE *file)
{
    int len_sec = len_sections(file);
    int i;
    for (i = 0; i < len_sec; i++)
    {
        if (file->sections_tab[i].header.sh_type == SHT_SYMTAB)
        {
            return i;
        }
    }
    return -1;
}

int len_symbols(ELF32_FILE *file)
{
    int index = symbols_table_index(file);
    if (index == -1)
    {
        return 0;
    }
    return file->sections_tab[index].header.sh_size / file->sections_tab[index].header.sh_entsize;
}

ERR_CODE read_symbol_tab(ELF32_FILE *file, FILE *stream)
{

    int sym_count = len_symbols(file);
    
    if (sym_count == 0) { return OK; }

    Elf32_Off offset = file->sections_tab[symbols_table_index(file)].header.sh_offset;


    Elf32_Sym *symbols = malloc(sym_count * sizeof(Elf32_Sym));
    if (!symbols)
    {
        error("Erreur d'allocation mémoire pour les symboles\n");
        return ERR_ALLOC;
    }

    // Deplacement du curseur
    fseek(stream, offset, SEEK_SET);

    // Lecture des symboles
    for (int i = 0; i < sym_count; i++)
    {
        if (get_u32(stream, &symbols[i].st_name) ||
            get_u32(stream, &symbols[i].st_value) ||
            get_u32(stream, &symbols[i].st_size) ||
            get_u8(stream, &symbols[i].st_info) ||
            get_u8(stream, &symbols[i].st_other) ||
            get_u16(stream, &symbols[i].st_shndx))
        {
            error("Erreur lors de la lecture d'un champ de la table des symboles\n");
            free(symbols);
            return ERR_READ;
        }
    }

    // Stocker les informations dans la structure ELF32_FILE
    file->symbols_tab = symbols;

    int len_sec = len_sections(file);
    for (int i = 0; i < len_sec; i++)
    {

        Elf32_Shdr *section_hdr = &file->sections_tab[i].header;

        if (section_hdr->sh_type == SHT_STRTAB && i != file->header.e_shstrndx)
        {
            if (fseek(stream, section_hdr->sh_offset, SEEK_SET) != 0)
            {
                error("Failed to seek to .strtab section offset\n");
                exit(EXIT_FAILURE);
            }
            file->symbol_names = malloc(section_hdr->sh_size + 1);
            if (file->symbol_names == NULL)
            {
                error("Memory allocation failed for symbols_names\n");
                exit(EXIT_FAILURE);
            }
            if (fread(file->symbol_names, section_hdr->sh_size, 1, stream) != 1)
            {
                error("Failed to read .strtab section\n");
                free(file->symbol_names);
                exit(EXIT_FAILURE);
            }
            return OK;
        }
    }

    return OK;
}

ERR_CODE __read_reimp_tab_rel(ELF32_FILE *file, Elf32_Relocation *Tab, FILE *stream)
{
    // Lecture des relocations
    int i = 0;
    for (int k = 0; k < len_sections(file); k++)
    {

        if (file->sections_tab[k].header.sh_type == SHT_REL)
        {

            // Initialisation de l'identité de la section
            Tab->tab[i].name = file->sections_tab[k].header.sh_name;
            Tab->tab[i].offset = file->sections_tab[k].header.sh_offset;
            Tab->tab[i].size = file->sections_tab[k].header.sh_size;
            Tab->tab[i].nbr_entries = file->sections_tab[k].header.sh_size / file->sections_tab[k].header.sh_entsize;
            Tab->tab[i].index = k;

            // Lecture
            Tab->tab[i].tab = malloc(Tab->tab[i].nbr_entries * sizeof(__Elf32Rel));
            if (!Tab->tab[i].tab)
            {
                error("Erreur d'allocation mémoire pour les relocations\n");
                free(Tab->tab);
                free(Tab);
                return ERR_ALLOC;
            }

            fseek(stream, Tab->tab[i].offset, SEEK_SET);

            for (int j = 0; j < Tab->tab[i].nbr_entries; j++)
            {
                if (get_u32(stream, &Tab->tab[i].tab[j].r_offset) ||
                    get_u32(stream, &Tab->tab[i].tab[j].r_info))
                {

                    error("Erreur lors de la lecture d'un champ de la table des relocations\n");
                    free(Tab->tab);
                    free(Tab);
                    return ERR_READ;
                }

                // Initialisation de "sh_link" et "sh_info"
                Tab->tab[i].tab[j].sh_link = file->sections_tab[k].header.sh_link;
                Tab->tab[i].tab[j].sh_info = file->sections_tab[k].header.sh_info;  
            }
            /*fprintf(stderr, "indice i = %d\n", i);*/
            i++;
        }
    }

    return OK;
}

ERR_CODE read_reimp_tab(ELF32_FILE *file, FILE *stream)
{
    if (!file->sections_tab)
    {
        error("\nla table des sections n'est pas chargée\n");
        return ERR_READ;
    }

    Elf32_Relocation *Tab;
    Tab = malloc(sizeof(Elf32_Relocation));
    Tab->len_tab = 0;
    if (!Tab)
    {
        error("Erreur d'allocation mémoire pour la table des relocations\n");
        return ERR_ALLOC;
    }

    // initialisation des tailles
    for (int i = 0; i < len_sections(file); i++)
    {
        if (file->sections_tab[i].header.sh_type == SHT_REL)
            Tab->len_tab++;
    }

    // Allocation de memoire
    /*fprintf(stderr, "len = %d\n", Tab->len_tab);*/

    Tab->tab = malloc(Tab->len_tab * sizeof(Elf32Rel));
    if (!Tab->tab)
    {
        error("Erreur d'allocation mémoire pour la table des relocations\n");
        free(Tab);
        return ERR_ALLOC;
    }

    // Lecture des relocations
    if (__read_reimp_tab_rel(file, Tab, stream) != OK)
    {
        error("Erreur lors de la lecture des relocations\n");
        free(Tab->tab);
        free(Tab);
        return ERR_READ;
    }

    // Stocker les informations dans la structure ELF32_FILE
    file->reimp_tab = Tab;
    return OK;
}

/********
 *
 * Fonctions d'écriture
 */

ERR_CODE write_elf_header(ELF32_FILE *file, FILE *stream)
{

    fseek(stream, 0, SEEK_SET);
    Elf32_Ehdr *header = &file->header;

    for (int i = 0; i < EI_NIDENT; i++)
    {
        set_u8(stream, header->e_ident[i]);
    }

    // Ecriture des autres champs de l'en-tête ELF
    if (set_u16(stream, header->e_type) ||
        set_u16(stream, header->e_machine) ||
        set_u32(stream, header->e_version) ||
        set_u32(stream, header->e_entry) ||
        set_u32(stream, header->e_phoff) ||
        set_u32(stream, header->e_shoff) ||
        set_u32(stream, header->e_flags) ||
        set_u16(stream, header->e_ehsize) ||
        set_u16(stream, header->e_phentsize) ||
        set_u16(stream, header->e_phnum) ||
        set_u16(stream, header->e_shentsize) ||
        set_u16(stream, header->e_shnum) ||
        set_u16(stream, header->e_shstrndx))
    {
        error("Erreur lors de l'écriture du champ de l'en-tête ELF\n");
        return ERR_WRITE;
    }
    return OK;
}

ERR_CODE write_section_tab_header(ELF32_FILE *file, FILE *stream)
{
    Elf32_ShdrFull *sections = file->sections_tab;

    // Deplacement du curseur
    fseek(stream, offset_sections(file), SEEK_SET);

    // Ecriture des en-têtes de section
    for (int i = 0; i < file->old_len_sections; i++)
    {
        if (sections[i].header.sh_type == SHT_REL)
        {
            continue;
        }
        if (set_u32(stream, sections[i].header.sh_name) ||
            set_u32(stream, sections[i].header.sh_type) ||
            set_u32(stream, sections[i].header.sh_flags) ||
            set_u32(stream, sections[i].header.sh_addr) ||
            set_u32(stream, sections[i].header.sh_offset) ||
            set_u32(stream, sections[i].header.sh_size) ||
            set_u32(stream, sections[i].header.sh_link) ||
            set_u32(stream, sections[i].header.sh_info) ||
            set_u32(stream, sections[i].header.sh_addralign) ||
            set_u32(stream, sections[i].header.sh_entsize))
        {
            error("Erreur lors de l'écriture d'un champ de de la table des sections\n");
            return ERR_WRITE;
        }
    }

    return OK;
}

ERR_CODE write_symbol_tab(ELF32_FILE *file, FILE *stream)
{

    int sym_count = len_symbols(file);

    Elf32_Sym *symbols = file->symbols_tab;

    // Ecriture des symboles
    for (int i = 0; i < sym_count; i++)
    {
        if (set_u32(stream, symbols[i].st_name) ||
            set_u32(stream, symbols[i].st_value) ||
            set_u32(stream, symbols[i].st_size) ||
            set_u8(stream, symbols[i].st_info) ||
            set_u8(stream, symbols[i].st_other) ||
            set_u16(stream, symbols[i].st_shndx))
        {
            error("Erreur lors de l'écriture d'un champ de la table des symboles\n");
            free(symbols);
            return ERR_WRITE;
        }
    }

    return OK;
}

ERR_CODE write_section_content(ELF32_FILE *file, FILE *stream_output)
{

    int res;

    for (int i = 0; i < file->old_len_sections; i++)
    {

        Elf32_ShdrFull *current_section = &file->sections_tab[i];

        res = fseek(stream_output, current_section->header.sh_offset, SEEK_SET);

        if (res != 0)
        {
            error("can't fseek\n");
            return ERR_WRITE;
        }


        // on ecrit la table des symboles depuis les structures en C
        if (strcmp(".symtab", &file->section_names[current_section->header.sh_name]) == 0)
        {
            write_symbol_tab(file, stream_output);
        }
        // Le reste grâce a "content"
        else
        {
            res = fwrite(current_section->content, 1, current_section->header.sh_size, stream_output);

            if (res != current_section->header.sh_size)
            {
                return 1;
            }
        }

    }


    return OK;
}

ERR_CODE write_file(ELF32_FILE *file, FILE *stream_output)
{

    // must be called first to write the offset in file
    if (
        write_section_content(file, stream_output) != OK)
    {

        error("Erreur write_section_content\n");
        return EXIT_FAILURE;
    }

    if (
        write_elf_header(file, stream_output) != OK ||
        write_section_tab_header(file, stream_output) != OK)
    {
        return EXIT_FAILURE;
    }

    return OK;
}

/****
 * Fin fonctions d'écriture
 */


// Étape 6 : Renumérotation des sections
ERR_CODE __remov_rel(ELF32_FILE *file)
{

    // Construction d'une nouvelle table des sections
    /*Elf32Rel *asuprimer = file->reimp_tab->tab;*/
    int taille_asuprimer = file->reimp_tab->len_tab;

    int new_len_sections = (len_sections(file) - taille_asuprimer);

    // mettre a jour sh_link
    for (int i = 0; i < len_sections(file); i++)
    {

        Elf32_ShdrFull *current_section = &file->sections_tab[i];

        if (current_section->header.sh_type == SHT_REL)
        {
            continue;
        }

        Elf32_Shdr previous_link_section = file->sections_tab[current_section->header.sh_link].header;

        int real_index = 0;
        for (int j = 0; j < len_sections(file); j++)
        {
            if (file->sections_tab[j].header.sh_type != SHT_REL)
            {

                // on utilise "sh_name" comme id de section, car "logiquement", le nom est unique
                if (file->sections_tab[j].header.sh_name == previous_link_section.sh_name)
                {
                    current_section->header.sh_link = real_index;
                    break;
                }

                real_index++;
            }
        }
    }

    // mettre a jour st_shndx dans la table des symboles
    int len_sym = len_symbols(file);
    for (int i = 0; i < len_sym; i++)
    {
        Elf32_Sym *current_symbol = &file->symbols_tab[i];
        debug("current_symbol->st_shndx = %d\n", current_symbol->st_shndx);
        if (current_symbol->st_shndx == SHN_ABS) {
            continue;
        }
        Elf32_ShdrFull *previous_header_section = &file->sections_tab[current_symbol->st_shndx];

        int real_index = 0;
        for (int j = 0; j < len_sections(file); j++)
        {

            if (file->sections_tab[j].header.sh_type != SHT_REL)
            {
                // on utilise "sh_name" comme id de section, car "logiquement", le nom est unique
                if (file->sections_tab[j].header.sh_name == previous_header_section->header.sh_name)
                {
                    file->symbols_tab[i].st_shndx = real_index;
                    break;
                }
                real_index++;
            }
        }
    }

    file->header.e_shnum = new_len_sections;
    // todo: change this to the index of .shstrtab
    file->header.e_shstrndx = new_len_sections - 1;

    /*file->header.e_type = ET_EXEC;*/

    return OK;
}

// Étape 7 : Correction des symboles
ERR_CODE __adrresse_absolue_sections(ELF32_FILE *file)
{
    Elf32_ShdrFull *sections = file->sections_tab;
    int taille = len_sections(file);
    Elf32_Addr current_txt_address = BASE_TEXT_ADDRESS;
    Elf32_Addr current_data_address = BASE_DATA_ADDRESS;
    

    for (int i = 0; i < taille; i++)
    {
        // SHF_ALLOC(bit d'allocation) indique que la section doit être incluse dans l'espace mémoire virtuel de l'exécutable.)
        if (sections[i].header.sh_flags & SHF_ALLOC)
        {
            // Appliquer l'alignement requis (Cette section doit recevoir une nouvelle adresse)
            Elf32_Addr alignment = sections[i].header.sh_addralign;

            if (alignment > 0)
            {
                // Aligner l'adresse courante pour qu'elle soit multiple de "sh_addralign"
                current_txt_address = (current_txt_address + alignment - 1) & ~(alignment - 1);
                current_data_address = (current_data_address + alignment - 1) & ~(alignment - 1);
            }
            
            if (strcmp(".text", &file->section_names[file->sections_tab[i].header.sh_name]) == 0)
            {
                // Affecter la nouvelle adresse
                file->sections_tab[i].header.sh_addr = current_txt_address;

                // Mise à jour de l'adresse courante
                current_txt_address += sections[i].header.sh_size;
            }

            else if (strcmp(".data", &file->section_names[file->sections_tab[i].header.sh_name]) == 0){
                // Affecter la nouvelle adresse
                file->sections_tab[i].header.sh_addr = current_data_address;

                // Mise à jour de l'adresse courante
                current_data_address += sections[i].header.sh_size;
            }
        }
    }
    return OK;
}

ERR_CODE __correction_value_symbols(ELF32_FILE *file)
{
    // Récupérer la table des symboles
    int taille_sym = len_symbols(file);
    Elf32_ShdrFull *sections = file->sections_tab;

    // Parcourir la table des symboles
    for (int i = 0; i < taille_sym; i++)
    {

        // Identifier les symboles traduisibles (Vérifier si le symbole est translatable et n'est pas déjà déclaré)
        if (file->symbols_tab[i].st_shndx != SHN_UNDEF && file->symbols_tab[i].st_shndx != SHN_ABS)
        {

            // Mettre à jour le champ st_value(adresse absolue)
            Elf32_Addr section_address = sections[file->symbols_tab[i].st_shndx].header.sh_addr;
            file->symbols_tab[i].st_value += section_address;

        }
    }
    return OK;
}

// Étape 8 : Réimplantations de type R ARM ABS*
//
// (si j'ai bien compris), le concept de relocation est de remplacé les adresses inconnues dans les sections
// .text et .data par des adresses que l'on calcule dans cette fonction.
// Les "trous" (qui sont des 0000 dans le fichier .o), sont listés dans la table de relocation.
// En fonction du type de relocation, on va calculé l'adresse differement.
// On se sert de la table des symboles, car elle contient toute les variables global & functions (ce qui peut être référencé dans d'autres fichiers)
//
ERR_CODE __reimplantation_R_ARM_ABS(ELF32_FILE *file)
{
    // Récupérer des informations
    Elf32_ShdrFull *sections = file->sections_tab;
    Elf32_Relocation *reloc = file->reimp_tab;
    Elf32_Sym *symbols = file->symbols_tab;

    // Parcours des tables de relocations
    for (int i = 0; i < reloc->len_tab; i++)
    {

        int p = sections[reloc->tab[i].index].header.sh_info;
        // int p = 3;
        debug("p = %#x\n", p);
        
        // Recuperation de la section cible
        Elf32_ShdrFull *section_cible = &sections[p]; // "sh_info" de la section de type  REL nous donne l'indice de la section cible

        debug("Section cible %s\n", &file->section_names[section_cible->header.sh_name]);
        
        // Parcours et Recuperation des entrées de la table de relocation en cours :
        for (int j = 0; j < reloc->tab[i].nbr_entries; j++)
        {
            //  ELF32_R_SYM(i)  Extrait l'index du symbole
            //  ELF32_R_TYPE(i) Extrait le type de réimplantation

            int index_symbole = ELF32_R_SYM(reloc->tab[i].tab[j].r_info); //(index) Une référence à un symbole : le symbole à l'origine de la réimplantation dans la table des symboles.
            Elf32_Addr position = reloc->tab[i].tab[j].r_offset / 4;          // Position de l'adresse à modifier  dans la section.
            Elf32_Addr val_actuelle;                                      // valeur déjà présente à l'emplacement "position" dans la section.
            Elf32_Addr val_symbole = symbols[index_symbole].st_value;     // L'adresse absolue du symbole référencé, corrigée lors de l'étape 7
            Elf32_Addr new_val;                                           // val_actuelle + val_symbole : La nouvelle valeur à écrire à l'adresse "position "dans la section

            // Vérification de type
            switch (ELF32_R_TYPE(reloc->tab[i].tab[j].r_info))
            {
            // (S + A) | T
            case R_ARM_ABS32:
                val_actuelle = reverse_4(section_cible->content[position]);
                new_val = val_actuelle + val_symbole;
                file->sections_tab[p].content[position] = reverse_4(new_val);
                break;

            case R_ARM_ABS16:
                debug("R_ARM_ABS16\n");
                val_actuelle = reverse_2(section_cible->content[position]);
                new_val = val_actuelle + val_symbole;
                // Copier la nouvelle valeur à l'emplacement "position_eff" dans la section
                section_cible->content[position] = reverse_2(new_val);

                break;

            case R_ARM_ABS8:

                section_cible->content[position] += val_symbole;
                break;

            // ((S + A) | T) – P
            case R_ARM_JUMP24:
            case R_ARM_PC24:
            case R_ARM_CALL:
                debug("R_ARM_CALL\n");
                val_actuelle = reverse_4(section_cible->content[position]);
                new_val = val_actuelle + val_symbole - position;
                section_cible->content[position] = reverse_4(new_val);
               
                break;

            default:
                break;
            }
        }
    }

    return OK;
}

ERR_CODE restructuration_section(ELF32_FILE *file, FILE *stream, char *output_path, uint8_t etape)
{
    if (!file->reimp_tab)
    {
        error("La table des reimplantations n'est pas chargée\n");
        return ERR_READ;
    }

    /* Étape 7 : Correction des symboles */
    if (etape > 6 || etape == 0) {
        debug("Lancement de l'étape 7\n");
        if (__adrresse_absolue_sections(file) != OK ||
            __correction_value_symbols(file) != OK)
        {
            error("Erreur lors de la Correction des symboles\n");
            return ERR_READ;
        }
    }

    // /* Étape 8 : Réimplantations de type R ARM ABS* */
    if (etape > 7 || etape == 0) {
        debug("Lancement de l'étape 8 et 9\n");
        __reimplantation_R_ARM_ABS(file);

    }

    // on supprime a la fin pcq ca cause des probleme dans la reimplantation
    /* Étape 6 : Renumérotation des sections */
    if (etape == 6 || etape == 0) {
        if (__remov_rel(file) != OK)
        {
            error("Erreur lors de la modification du fichier\n");
            return ERR_READ;
        }
    }
    // Ouvrir le fichier de sortie en mode binaire pour ne pas change les \n
    FILE *stream_output = Fopen(output_path, "wb");
    if (!stream_output)
    {
        error("Erreur lors de l'ouverture du fichier de sortie\n");
        return ERR_ALLOC;
    }

    if (write_file(file, stream_output) != OK)
    {

        error("Erreur lors de l'écriture du fichier\n");
        fclose(stream_output);
        return ERR_WRITE;
    }

    fclose(stream_output);

    return OK;
}

Elf32_ShdrFull *find_section(ELF32_FILE *file, char *section_name)
{

    for (int i = 0; i < len_sections(file); i++)
    {
        if (strcmp(section_name, &file->section_names[file->sections_tab[i].header.sh_name]) == 0)
        {
            return &file->sections_tab[i];
        }
    }
    return NULL;
}

Elf32_ShdrFull *get_section_by_index(ELF32_FILE *file, uint32_t index)
{

    int index_sec = 0;
    int mem_index = 0;

    while (index_sec != index)
    {
        assert(mem_index < file->old_len_sections);
        if (file->sections_tab[mem_index].header.sh_type == SHT_REL)
        {
            mem_index++;
        }
        else
        {
            index_sec++;
            mem_index++;
        }
    }

    assert(mem_index < file->old_len_sections);

    return &file->sections_tab[mem_index];
}
