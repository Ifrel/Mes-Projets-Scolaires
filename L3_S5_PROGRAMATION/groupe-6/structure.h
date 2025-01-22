#ifndef __STRUCTURE__
#define __STRUCTURE__

#include <elf.h>
#include <stdio.h>
#include <stdbool.h>
#include "status.h"

#define BASE_TEXT_ADDRESS (uint32_t) 0x58
#define BASE_DATA_ADDRESS (uint32_t) 0x1000

/***************************************************
 * *    Définition de la structure de réalocation * *
 ****************************************************/
typedef struct
{
  Elf32_Addr r_offset; // Offset de la réimplantation (adresse à modifier)
  Elf32_Word r_info;   // Type de réimplantation et index de symbole
  Elf32_Word sh_link;  /* Liens vers Une table des symboles (via sh_link dans l'en-tête de section */
  Elf32_Word sh_info;  /* Une section à modifier (via sh_info dans l'en-tête de section) */
} __Elf32Rel;

typedef struct
{
  Elf32_Word name;  /* Nom de la section de relocations */
  Elf32_Off offset; /* Offset de la section de relocations */
  int nbr_entries;  /* Nombre d'élément dans la table de relocations */
  int index;        /* Index de la table de relocations (dans la table des sections) */
  Elf32_Word size;  /* Taille de la section en octets */
  __Elf32Rel *tab;  /* Table de relocations */
} Elf32Rel;

typedef struct
{
  Elf32Rel *tab;
  int len_tab;
} Elf32_Relocation;

typedef struct
{
  Elf32_Shdr header;
  // blob de data directement lu depuis le fichier source.
  // il est donc normalement déjà dans la bonne endianness
  // il y a une exception pour le tableau des symboles, cette variable
  // ne represente rien dans ce cas.
  // Important: le code fait l'asumption que la taille du tableau des symboles ne
  // change pas lorsquelle est modifiée.
  uint32_t *content;
} Elf32_ShdrFull;

/*************************************************************
 * Structure pour stocker les informations des differantes    *
 * parties dans un fichier ELF32                              *
 **************************************************************/
typedef struct
{
  // Information sur le contenu du fichier
  Elf32_Ehdr header;

  Elf32_ShdrFull *sections_tab;
  /**
   * Les noms sont ecrit cote a cote. On peut acceder
   * a un nom en utilisant `sh_name`, qui nous donne l'offset du debut du nom.
   */
  char *section_names;

  char *symbol_names;

  Elf32_Sym *symbols_tab;
  Elf32_Relocation *reimp_tab;

  int taille_fichier;
  int old_len_sections;

} ELF32_FILE;

Elf32_Half len_sections(ELF32_FILE *file);
int len_symbols(ELF32_FILE *file);
int symbols_table_index(ELF32_FILE *file);

ERR_CODE read_file(FILE *stream, ELF32_FILE *file);

void init_file(ELF32_FILE *file);

/**
 * Cette fonction libere seulement la memoire alloué par ce module
 * pas le top level `ELF32_FILE`.
 */
void free_file(ELF32_FILE *file);

ERR_CODE read_elf_header(ELF32_FILE *file, FILE *stream);

ERR_CODE read_section_tab(ELF32_FILE *file, FILE *stream);

ERR_CODE read_symbol_tab(ELF32_FILE *file, FILE *stream);

ERR_CODE read_reimp_tab(ELF32_FILE *file, FILE *stream);

/**
 * Fonction pour supprimer des sectionss de type REL et RELA et réstructurer fichier ELF32
 * @param file : La structure contenant les informations
 * @param stream : Le flux de données du nouveau fichier ELF32 */
ERR_CODE restructuration_section(ELF32_FILE *file, FILE *stream, char *output_path, uint8_t etape);



Elf32_ShdrFull *find_section(ELF32_FILE *file, char *section_name);
// will ignore section marked as should_remove
Elf32_ShdrFull * get_section_by_index(ELF32_FILE *file, uint32_t index);

#endif /* __NOTRE_STRUCTURE__ */
