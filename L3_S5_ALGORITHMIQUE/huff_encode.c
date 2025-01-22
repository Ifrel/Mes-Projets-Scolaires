
#include "arbrebin.h"
#include "bfile.h"
#include "fap.h"
#include "huffman_code.h"
#include <assert.h>
#include <stdio.h>

#define TAILLE_MAX 256
int TAILLE_TABLE_OCC = 0;

typedef struct {
    int tab[TAILLE_MAX];
} TableOcc_t;

struct code_char HuffmanCode[256];
struct code_char caractere;
char s[100];

void ConstruireTableOcc(FILE* fichier, TableOcc_t* TableOcc) {
    int c;

    for (int i = 0; i < TAILLE_MAX; i++) {
        TableOcc->tab[i] = 0;
    }

    c = fgetc(fichier);
    while (c != EOF) {
        TableOcc->tab[c]++;
        c = fgetc(fichier);
    };

    int i;
    printf("Occurences des caracteres: \n");
    printf("Caractère  code   Occurance: \n");
    for (i = 0; i < TAILLE_MAX; i++) {
        if (TableOcc->tab[i] != 0) {
            printf("%c    %d     %d\n", i, i, TableOcc->tab[i]);
            TAILLE_TABLE_OCC++;
        }
    }
}

fap InitHuffman(TableOcc_t* TableOcc) {
    fap f = creer_fap_vide();
    for (int i = 0; i < TAILLE_MAX; i++) {
        if (TableOcc->tab[i] != 0) {
            Arbre a = NouveauNoeud(ArbreVide(), i, ArbreVide());
            f = inserer(f, a, TableOcc->tab[i]);
        }
    }
    return f;
}

Arbre ConstruireArbre(fap file) {
    int file_not_null = 1;
    Arbre a, b, c, resultat;
    int p1, p2;

    if (file->prochain == NULL) {
        extraire(file, &a, &p1);
        c = NouveauNoeud(a, ' ', ArbreVide());
        return c;
    }

    for (int i = 1; i < TAILLE_TABLE_OCC && file_not_null; i++) {
        file = extraire(file, &a, &p1);

        // Vérification si l'extraction a réussi
        if (file == NULL)
            file_not_null = 0; // Si le fichier est vide, on met à jour le flag

        if (file_not_null) {
            file = extraire(file, &b, &p2);
            c = NouveauNoeud(a, ' ', b);
            file = inserer(file, c, p1 + p2);
        }
    }

    // Extraire le dernier nœud qui sera la racine de l'arbre Huffman
    file = extraire(file, &resultat, &p1);

    return resultat;
}

char* tab_int_To_char(struct code_char t) {
    for (int i = 0; i < t.lg; i++) {
        if (t.code[i] == 0) {
            s[i] = '0';
        } else {
            s[i] = '1';
        }
    }
    s[t.lg] = '\0';
    return s;
}
void ConstruireCode(Arbre huff) {
    if (EstVide(huff))
        return;
    if (EstVide(huff->fg) && EstVide(huff->fd)) {
        HuffmanCode[huff->etiq] = caractere;
        fprintf(stderr,
                "Huffman[%c] =  %s ( taille : %d\n)",
                huff->etiq,
                tab_int_To_char(caractere),
                caractere.lg);
        return;
    }

    caractere.code[caractere.lg] = 0;
    (caractere.lg)++;
    ConstruireCode(huff->fg);

    (caractere.lg)--;
    caractere.code[caractere.lg] = 1;
    (caractere.lg)++;
    ConstruireCode(huff->fd);
    (caractere.lg)--;
}

void Encoder(FILE* fic_in, FILE* fic_out, Arbre ArbreHuffman) {
    ConstruireCode(ArbreHuffman);
    EcrireArbre(fic_out, ArbreHuffman);

    int c;
    BFILE* f = bstart(fic_out, "w");

    c = fgetc(fic_in);
    while (c != EOF) {
        for (int i = 0; i < HuffmanCode[c].lg; i++) {
            bitwrite(f, HuffmanCode[c].code[i]);
        }

        c = fgetc(fic_in);
    }

    bstop(f);
}

int main(int argc, char* argv[]) {
    TableOcc_t TableOcc;
    FILE* fichier;
    FILE* fichier_encode;

    fichier = fopen(argv[1], "r");
    if (fichier == NULL) {
        printf("Erreur d'ouverture du fichier '%s' en lecture\n", argv[1]);
        return 1;
    }
    /* Construire la table d'occurences */
    ConstruireTableOcc(fichier, &TableOcc);
    fclose(fichier);

    /* Initialiser la FAP */
    fap file = InitHuffman(&TableOcc);

    /* Afficher la FAP */
    // print_fap(file);

    /* Construire l'arbre d'Huffman */
    Arbre ArbreHuffman = ConstruireArbre(file);

    AfficherArbre(ArbreHuffman);

    /* Construire la table de codage */
    caractere.lg = 0;
    ConstruireCode(ArbreHuffman);

    /* Encodage */
    fichier = fopen(argv[1], "r");
    if (fichier == NULL) {
        printf("Erreur d'ouverture du fichier '%s' en lecture\n", argv[1]);
        return 1;
    }
    fichier_encode = fopen(argv[2], "w");
    if (fichier_encode == NULL) {
        printf("Erreur d'ouverture du fichier '%s' en écriture\n", argv[2]);
        fclose(fichier);
        return 1;
    }

    Encoder(fichier, fichier_encode, ArbreHuffman);

    fclose(fichier_encode);
    fclose(fichier);

    return 0;
}
