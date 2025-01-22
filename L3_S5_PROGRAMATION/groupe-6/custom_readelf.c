// executable similar to readelf
#include <elf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <stdbool.h>

#include "csapp.h"
#include "util.h"
#include "status.h"
#include "debug.h"
#include "structure.h"
#include "display.h"

void usage(char *argv[])
{
    fprintf(stderr, "usage : %s { [ -h | --file-header] | [ -S | --sections ] [ {-x | --hex-dump} {<index-section> | <nom-section>} ] }\n", argv[0]);
}

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        usage(argv);
        exit(1);
    }

    struct option longopts[] = {
        {"file-header", no_argument, NULL, 'h'},
        {"section", no_argument, NULL, 'S'},
        {"hex-dump", required_argument, NULL, 'x'},
        {"sym", no_argument, NULL, 's'},
        {"relocation", no_argument, NULL, 'r'},
        {"debug", no_argument, NULL, 'd'},
        {NULL, 0, NULL, 0}};

    int opt;
    char *section_name;

    // On peut pas definir en une ligne
    bool h = false;
    bool S = false;
    bool x = false;
    bool s = false;
    bool r = false;

    while ((opt = getopt_long(argc, argv, "hSx:srd", longopts, NULL)) != -1)
    {
        switch (opt)
        {
        case 'h':
            h = true;
            break;
        case 'S':
            S = true;
            break;
        case 'x':
            section_name = optarg;
            x = true;
            break;
        case 's':
            s = true;
            break;
        case 'r':
            r = true;
            break;
        case 'd':
            add_debug_to("custom_readelf.c");
            add_debug_to("structure.c");
            break;
        default:
            usage(argv);
            exit(1);
            break;
        }
    }

    // optind correspond à la position du premier argument qui n'est pas une option,
    // donc './custom_readelf FICHIER -s' marche aussi bien que './custom_readelf -s FICHIER'  
    FILE *stream = Fopen(argv[optind], "r");

    ELF32_FILE file;

    ERR_CODE ret;
    if ((ret = read_file(stream, &file)) != OK)
    {

        /*fprintf(stderr, "Erreur lors de la lecture de toutes les tables\n");*/
        if (ret == ERR_READ) {
            fprintf(stderr, "readelf: Error: %s: Failed to read file's magic number\n", argv[optind]);
        }
        fclose(stream);
        return ret;
    }

    if (h)
    {
        display_ELF_header(&file.header);
    }

    if (S)
    {
        display_ELF_section_table(&file);
    }

    if (x)
    {
        display_ELF_section_content(&file, section_name, stream);
    }

    if (s)
    {
        display_symbol_table(&file);
    }

    if (r)
    {
        display_relocation_table(&file);
    }

    fclose(stream);
    free_file(&file);
    return 0;
}
