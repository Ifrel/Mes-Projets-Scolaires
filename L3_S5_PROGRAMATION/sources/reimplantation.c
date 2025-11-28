#include <elf.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <stdbool.h>

#include "status.h"
#include "structure.h"
#include "display.h"
#include "csapp.h"
#include "debug.h"
#include "util.h"

// Affiche l'usage et la manière d'utiliser le programme
void usage(char *argv[])
{
    fprintf(stderr, "Usage: %s <fichier ELF> <Options> { -o }\n", argv[0]);
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        usage(argv);
        return EXIT_FAILURE;
    }

    struct option longopts[] = {
        {"output", required_argument, NULL, 'o'},
        {"etape", required_argument, NULL, 'e'},
        {"debug", no_argument , NULL, 'd'},
        {NULL, 0, NULL, 0}
    };

    char *output_path = "resultat";
    uint8_t etape = 0;
    int len;
    bool has_debug = false;

    int opt;
    while ((opt = getopt_long(argc, argv, "o:e:d", longopts, NULL)) != -1)
    {
        switch (opt)
        {
        // Change le nom du fichier de sortie. Par défaut: resultat.
        case 'o':
            output_path = optarg;
            break;
        
        // Execute uniquement une certaine étape de la phase 2.
        case 'e':
            // Vérifier que c'est bien un entier qui est donné en argument de -e
            len = strlen(optarg);
            for (int i = 0; i < len; i++) {
                if (! isdigit(optarg[i])) {
                    die(1, "-e '%s' n'est pas un entier\n", optarg);
                }
            }
            etape = atoi(optarg);
            if (etape > 11 || etape < 6) { die(1, "-e '%s' n'est pas compris entre 6 et 11 inclus\n", optarg); }
        
            break;

        case 'd':
            add_debug_to("reimplantation.c");
            add_debug_to("structure.c");
            has_debug = true;
            break;
            
        default:
            usage(argv);
            exit(1);
            break;
        }
    }
    

    char *input_path = argv[optind];
    FILE *stream = Fopen(input_path, "r");

    // Convertir notre fichier en structure ELF32_FILE pour faciliter sa manipulation
    ELF32_FILE file;

    debug("Leture du fichier '%s'\n", input_path);
    ERR_CODE ret;
    if ((ret = read_file(stream, &file)) != OK)
    {
        if (ret == ERR_READ) {
            fprintf(stderr, "readelf: Error: %s: Failed to read file's magic number\n", argv[optind]);
        }
        fclose(stream);
        return EXIT_FAILURE;
    }

    // Affiche la table des sections si nous sommes en mode développement. (option 'd')
    if (has_debug)
      display_ELF_section_table(&file);

    // Supprimer les sections de type REL et RELA et réstructurer le fichier ELF
    if (restructuration_section(&file, stream, output_path, etape) != OK)
    {
        error("Erreur lors de la réstructuration des sections\n");
        return EXIT_FAILURE;
    }

    fclose(stream);
    free_file(&file);

    return EXIT_SUCCESS;
}
