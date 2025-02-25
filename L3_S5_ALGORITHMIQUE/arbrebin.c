#include "arbrebin.h"
#include <stdio.h>
#include <stdlib.h>

/* implementation des operateurs de l'Arbre binaire */

/***************************************
 * ArbreVide                           *
 * parametres : aucun                  *
 * resultat : un Arbre                 *
 * description : renvoie un arbre vide *
 * effet de bord :aucun                *
 ***************************************/
Arbre ArbreVide() {
    return NULL;
}

/**********************************************************************
 * NouveauNoeud                                                       *
 * parametres : les donnees : un Arbre g, un caractere c, un Arbre d  *
 * resultat : un Arbre                                                *
 * description : renvoie un nouvel Arbre dont la racine est etiquetee *
 * par c, de fils gauche g et de fils droit d                         *
 * effet de bord : une nouvelle cellule est allouee                   *
 **********************************************************************/
Arbre NouveauNoeud(Arbre g, Element c, Arbre d) {
    Arbre a = (Arbre)malloc(sizeof(struct cellule));

    a->etiq = c;
    a->fg = g;
    a->fd = d;

    return a;
}

/********************************************
 * EstVide                                  *
 * parametres : un Arbre a                  *
 * resultat : un booleen                    *
 * description : renvoie vrai si a est vide *
 * effet de bord : aucun                    *
 ********************************************/
int EstVide(Arbre a) {
    return (a == NULL);
}

/******************************************
 * Etiq                                   *
 * parametres : un Arbre a                *
 * precondition : a non vide              *
 * resultat : un caractere                *
 * description : renvoie l'etiquette de a *
 * effet de bord : aucun                  *
 ******************************************/
Element Etiq(Arbre a) {
    return a->etiq;
}

/*********************************************
 * FilsGauche                                *
 * parametres : un Arbre a                   *
 * precondition : a non vide                 *
 * resultat : un arbre                       *
 * description : renvoie le fils gauche de a *
 * effet de bord : aucun                     *
 *********************************************/
Arbre FilsGauche(Arbre a) {
    return a->fg;
}

/********************************************
 * FilsDroit                                *
 * parametres : un Arbre a                  *
 * precondition : a non vide                *
 * resultat : un arbre                      *
 * description : renvoie le fils droit de a *
 * effet de bord : aucun                    *
 ********************************************/
Arbre FilsDroit(Arbre a) {
    return a->fd;
}

/********************************************
 * LibererArbre                             *
 * parametres : un Arbre a                  *
 * resultat : aucun                         *
 * description : libere l'arbre a           *
 * effet de bord : a est detruit            *
 ********************************************/
void LibererArbre(Arbre a) {
    if (!EstVide(a)) {
        LibererArbre(FilsGauche(a));
        LibererArbre(FilsDroit(a));
        free(a);
    }
}

/**********************************************************************
 * EcrireArbre                                                        *
 * parametres : un descripteur de fichier, un Arbre a                 *
 * precondition : le fichier est ouvert en ecriture                   *
 * resultat : aucun                                                   *
 * description : ecrit l'arbre a dans le fichier, sous la forme d'une *
 * sequence de triplets (entier, entier representant l'arbre gauche,  *
 *  entier representant l'arbre droit). L'arbre vide est represente   *
 * par 511. La sequence est terminee par 511.                             *
 * effet de bord : ecriture dans le fichier                           *
 **********************************************************************/
int cpt_noeud;

int EcrireArbreRec(FILE* fichier, Arbre a) {
    int fg, fd;
    int racine;

    if (EstVide(a)) {
        /* Ne rien ecrire, renvoyer 511 */
        return 511;
    } else {
        /* Ecrire le fils gauche, fg = caractere a la racine du fils gauche */
        fg = EcrireArbreRec(fichier, FilsGauche(a));
        /* Ecrire le fils droit, fd = caractere a la racine du fils droit */
        fd = EcrireArbreRec(fichier, FilsDroit(a));
        /* Ecrire la sequence (caractere du noeud courant, fg, fd) */
        if (fg == 511) {
            /* Ecriture d'une feuille : le code de l'arbre est le code
               ascii du caractere */
            racine = (int)Etiq(a);
        } else {
            racine = cpt_noeud;
            cpt_noeud += 1;
        };
        fwrite(&racine, sizeof(int), 1, fichier);
        fwrite(&fg, sizeof(int), 1, fichier);
        fwrite(&fd, sizeof(int), 1, fichier);
        return racine;
    }
}

void EcrireArbre(FILE* fichier, Arbre a) {
    int cco = 511;
    cpt_noeud = 256;
    EcrireArbreRec(fichier, a);
    /* sequence terminee par 511 */
    fwrite(&cco, sizeof(int), 1, fichier);
}

/*********************************************************************
 * LireArbre                                                         *
 * parametres : un descripteur de fichier                            *
 * precondition : le fichier est ouvert en lecture, et contient a la *
 * position courante un arbre ecrit par EcrireArbre                  *
 * resultat : un arbre                                               *
 * description : lit l'arbre dans le fichier, dans lequel il a ete   *
 * ecrit par EcrireArbre.                                            *
 * effet de bord : le fichier a ete lu, un arbre a ete cree          *
 *********************************************************************/
Arbre LireArbre(FILE* fichier) {
    /* Stockage des noeuds crees */
    Arbre TabArbre[512];
    int entierlu, fg, fd, racine;
    Element etiq;

    /* Initialisation : TabArbre[511] contient l'arbre vide */
    TabArbre[511] = ArbreVide();

    racine = 0;
    fread(&entierlu, sizeof(int), 1, fichier);
    while (entierlu != 511) {
#ifdef DEBUG_ARBRE
        fprintf(stderr, "entier lu : %d\n", entierlu);
#endif
        fread(&fg, sizeof(int), 1, fichier);
        fread(&fd, sizeof(int), 1, fichier);
#ifdef DEBUG_ARBRE
        fprintf(stderr, "fg : %d\n", fg);
        fprintf(stderr, "fd : %d\n", fd);
#endif
        /* Assertion : TabArbre[fg] et TabArbre[fd] ont ete affectes,
           eventuellement a l'arbre vide si fg=511 ou fd=511 */
        if (fg == 511) {
            etiq = (Element)entierlu;
        } else {
            etiq = 'a';
        };
        TabArbre[entierlu] = NouveauNoeud(TabArbre[fg], etiq, TabArbre[fd]);
        racine = entierlu;
        /* Noeud suivant */
        fread(&entierlu, sizeof(int), 1, fichier);
    }
    return TabArbre[racine];
}

/*********************************************************************
 * AfficherArbre                                                     *
 * parametres : un arbre a                                           *
 * resultat : aucun                                                  *
 * description : affiche l'arbre a sur la sortie standard            *
 * effet de bord : un arbre est affiche                              *
 *********************************************************************/
void AfficherArbreRec(Arbre a, int indent) {
    int i;

    if (!EstVide(a)) {
        for (i = 0; i < indent; i++) {
            fprintf(stderr, " ");
        }
        fprintf(stderr, "%c\n", Etiq(a));
        AfficherArbreRec(FilsGauche(a), indent + 2);
        AfficherArbreRec(FilsDroit(a), indent + 2);
    }
}

void AfficherArbre(Arbre a) {
    AfficherArbreRec(a, 0);
}

/*********************************************************************
 * AfficherArbre                                                     *
 *********************************************************************/

int EstFeuille(Arbre A) {
    if (EstVide(A)) {
        return 0;
    }
    return (FilsGauche(A) == NULL && FilsDroit(A) == NULL);
}

void affiche_arbre_aux(Arbre A, FILE* f) {
    if (!A) {
        return;
    }
    if (A->fg) {
        if (!EstFeuille(A->fg)) {
            fprintf(f,
                    "  \"%p\" [label=\"%c\", shape=circle]\n  \"%p\" "
                    "[label=\"%c\", "
                    "shape=circle]\n  \"%p\" -> \"%p\" [arrowhead=none, "
                    "label=0]\n\n",
                    A,
                    A->etiq,
                    A->fg,
                    A->fg->etiq,
                    A,
                    A->fg);
        } else {
            fprintf(f,
                    "  \"%p\" [label=\"%c\", shape=circle]\n  \"%p\" "
                    "[label=\"%c\", "
                    "shape=circle]\n  \"%p\" -> \"%p\" [arrowhead=none, "
                    "label=0]\n\n",
                    A,
                    A->etiq,
                    A->fg,
                    A->fg->etiq,
                    A,
                    A->fg);
        }
    }
    if (A->fd) {
        if (!EstFeuille(A->fd)) {
            fprintf(f,
                    "  \"%p\" [label=\"%c\", shape=circle]\n  \"%p\" "
                    "[label=\"%c\", "
                    "shape=circle]\n  \"%p\" -> \"%p\" [arrowhead=none, "
                    "label=1]\n\n",
                    A,
                    A->etiq,
                    A->fd,
                    A->fd->etiq,
                    A,
                    A->fd);
        } else {
            fprintf(f,
                    "  \"%p\" [label=\"%c\", shape=circle]\n  \"%p\" "
                    "[label=\"%c\", "
                    "shape=circle]\n  \"%p\" -> \"%p\" [arrowhead=none, "
                    "label=1]\n\n",
                    A,
                    A->etiq,
                    A->fd,
                    A->fd->etiq,
                    A,
                    A->fd);
        }
    }

    affiche_arbre_aux(A->fg, f);
    affiche_arbre_aux(A->fd, f);
}

// dot -Tpng -o BBB.png  Arbre.dot
void ArbreSympa(Arbre A) {
    FILE* f = fopen("Arbre.dot", "w");
    fprintf(f, "digraph ARBRE {\n");
    affiche_arbre_aux(A, f);
    fprintf(f, "}\n");
    fclose(f);
}
