#include "arbrebin.h"
#include "assert.h"
#include "bfile.h"
#include <stdio.h>

void Decoder(FILE* fichier_encode, FILE* fichier_decode, Arbre ArbreHuffman) {
    AfficherArbre(ArbreHuffman);
    BFILE* f = bstart(fichier_encode, "r");

    if (ArbreHuffman == NULL)
        return;
    else if (EstVide(ArbreHuffman->fg) && EstVide(ArbreHuffman->fd))
        fprintf(fichier_encode, "%c", ArbreHuffman->etiq);

    Arbre temp = ArbreHuffman;

    int c = bitread(f);
    while (c != EOF) {
        if (c == 0)
            temp = temp->fg;
        else
            temp = temp->fd;

        if (EstVide(temp->fg) && EstVide(temp->fd)) {
            fprintf(fichier_decode, "%c", temp->etiq);
            temp = ArbreHuffman;
        }

        c = bitread(f);
    }

    bstop(f);
}

int main(int argc, char* argv[]) {
    FILE* fichier_encode = fopen(argv[1], "r");
    FILE* fichier_decode = fopen(argv[2], "w");

    Arbre ArbreHuffman = LireArbre(fichier_encode);

    Decoder(fichier_encode, fichier_decode, ArbreHuffman);

    fclose(fichier_encode);
    fclose(fichier_decode);

    return 0;
}
