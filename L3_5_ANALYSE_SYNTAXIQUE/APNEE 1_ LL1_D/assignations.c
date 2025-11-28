//
// Created by Gilles Sérasset on 09/10/2019.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdarg.h>
#include "tokens.h"
#include "lookahead_lexer.h"
#include "uthash.h"

#include "assignations.h"


/*********** Managing Symbol Table with UTHash Hash table ***************/

struct Symbol {
    float value;
    char var[100];     /* we'll use this field as the key */
    UT_hash_handle hh; /* makes this structure hashable */
};

struct Symbol *symbols = NULL;

void set_value(char var[], float val) {
    struct Symbol* symbol;
    HASH_FIND_STR( symbols, var, symbol );
    if (symbol == NULL) {
        // the symbol does not exist yet
        symbol = calloc(1, sizeof(struct Symbol));
        strcpy(symbol->var, var);
        HASH_ADD_STR( symbols, var, symbol );
    }
    symbol->value = val;
}

float get_value(char var[]) {
    struct Symbol* symbol;
    HASH_FIND_STR( symbols, var, symbol );
    if (symbol == NULL) {
        return 0.;
    } else {
        return symbol->value;
    }
}

void print_symbols() {
    struct Symbol *s;

    for(s=symbols; s != NULL; s=s->hh.next) {
        fprintf(stdout, "%s = %f\n", s->var, s->value);
    }
}



/************** Grammaire *****************
*
*  S' → S $
*  S  → A ; S | ε
*  A  → x = A | E
*  E  → T E'
*  E' → + T E' | - T E' | ε
*  T  → U T'
*  T' → * U T' | / U T' | ε       si on voulais ajouter "%" : T' → * U T' | / U T' | % U T' | ε
*  U  → P U'
*  U' → ^ P U' | ε
*  P  → cos A | sin A | F          si on voulais ajouter "-/+" unaire: P  → + P | - P | cos A | sin A | F
*  F  → x | immediate | pi | (A)
*/


// S' → S $
void SOURCE_GRAMMAIRE() {
    DEBUG_MSG("SOURCE_GRAMMAIRE: Debut\n");
    float v = parseS();
    // -->> La verifiction si fin du fichier se fait dans le programme principal
    DEBUG_MSG("SOURCE_GRAMMAIRE: Fin, valeur = %f\n", v);
}


//  S  → A ; S | ε
float parseS() {
    DEBUG_ENTER("parseS");

    /* Si on arrive à la fin du fichier, production  S → ε (vide) */
    if (currentToken() == NULL) {
        DEBUG_MSG("parseS: Production vide (epsilon)\n");
        DEBUG_LEAVE("parseS");
        VERIFIER_SUIVANTS('S', currentToken());
        return 0.0f;
    }

    Token *t = currentToken();

    // Vérification : le premier token doit pouvoir commencer une assignation
    // PREMIER(S)    = { cos, sin, x, immediate, pi, ( }
    // (COSINE, SINE, VAR, IMMEDIATE, PI, OPAR )
    if (t->type == COSINE  || t->type == SINE  || t->type == VAR  ||
        t->type == IMMEDIATE || t->type == PI || t->type == OPAR) {

        // Production : S → A ; S
        DEBUG_MSG("parseS: Analyse d'une assignation/expression\n");

        // 1. Analyser l'assignation
        float resultat = parseA();
        DEBUG_VAL("parseS: Assignation/eval", resultat);

        // 2. Consommer le point-virgule séparateur
        consume(SEMICOLON);
        DEBUG_MSG("parseS: ; consommé\n");
        RESET_INDENT();

        // 3. Analyser la suite des assignations (récursion)
        parseS();

        DEBUG_LEAVE("parseS");
        VERIFIER_SUIVANTS('S', currentToken());
        return resultat;
        }
    else {
        // Token inattendu = erreur
        DEBUG_MSG(
            RED "!!! ERREUR !!! " RESET
            "Token inattendu dans parseS : " RED "\"%s\" " RESET
            "à la place de "
            GREEN "\"cos\"" RESET ", "
            GREEN "\"sin\"" RESET ", "
            GREEN "\"x\"" RESET ", "
            GREEN "\"immediate\"" RESET ", "
            GREEN "\"pi\"" RESET ", "
            GREEN "\"(\"" RESET
            "\n",
            t->text
        );

        fprintf(stdout, "!!! ERREUR !!!\n");
        exit(1);
    }
}



//  A  → x = A | E
float parseA() {
    DEBUG_ENTER("parseA");

    Token *t = currentToken();
    float resultat = 0.0f;

    // Cas 1 : Production A → x = A (assignation)
    // On vérifie que le 2ème token est ASSIGN pour distinguer de "E → x"
    if (t->type == VAR && lookup(2) != NULL && lookup(2)->type == ASSIGN) {
        DEBUG_MSG("parseA: Assignation detectee\n");

        // Vérification si x est une valeur imédiate
        if (t->type == IMMEDIATE) {
            DEBUG_MSG(
               RED "!!! ERREUR !!! " RESET
               "Affectation à une valeur immédiate : " RED "\"%d = %d\" " RESET "\n",
                t->value, t->value
            );
            fprintf(stdout, "!!! ERREUR !!!\n");
            exit(1);
        }

        // Sauvegarder le nom de la variable avant de la consommer
        char *nom_variable = get_text(t);
        DEBUG_MSG("parseA: Variable = %s\n", nom_variable);

        // Consommer le token VAR et le token ASSIGN
        consume(VAR);
        consume(ASSIGN);

        // Évaluer la partie droite (récursif pour gérer par exemple x = y = z)
        resultat = parseA();
        DEBUG_MSG("parseA: Valeur calculee pour %s = %f\n", nom_variable, resultat);

        // Stocker la valeur dans la table des symboles
        set_value(nom_variable, resultat);

        // Libérer la mémoire du nom de variable
        free(nom_variable);
        VERIFIER_SUIVANTS('A', currentToken());
        return resultat;
    }

    // Cas 2 : Production A → E (expression simple)
    else {
        DEBUG_MSG("parseA: Expression (pas d'assignation)\n");
        resultat = parseE();
    }

    DEBUG_LEAVE("parseA");
    VERIFIER_SUIVANTS('A', currentToken());
    return resultat;
}



//  E  → T E'
float parseE() {
    DEBUG_ENTER("parseE");

    /* Évaluer le premier terme */
    float resultat = parseT();
    DEBUG_VAL("parseE: Premier terme", resultat);

    /* Traiter les additions/soustractions restantes */
    resultat = parseE_prime(resultat);
    DEBUG_VAL("parseE: resultat final", resultat);

    DEBUG_LEAVE("parseE");
    VERIFIER_SUIVANTS('E', currentToken());
    return resultat;
}



//  E' → + T E' | - T E' | ε
float parseE_prime(float valeur_a_gauche) {
    DEBUG_ENTER("parseE_prime");
    DEBUG_VAL("valeur_a_gauche", valeur_a_gauche);

    // Fin du fichier ou production vide
    if (currentToken() == NULL) {
        DEBUG_MSG("parseE_prime: Fin de fichier (epsilon)\n");
        DEBUG_LEAVE("parseE_prime");
        VERIFIER_SUIVANTS('e', currentToken());
        return valeur_a_gauche;
    }

    Token *t = currentToken();
    float resultat = valeur_a_gauche;

    // Production: E' → + T E'
    if (t->type == PLUS) {
        DEBUG_MSG("parseE_prime: Operation PLUS detectee\n");
        consume(PLUS);

        /* Évaluer l'opérande droit */
        float valeur_a_droite = parseT();
        DEBUG_VAL("parseE_prime: Operande droite", valeur_a_droite);

        /* Calculer le résultat intermédiaire */
        resultat = valeur_a_gauche + valeur_a_droite;
        DEBUG_MSG("parseE_prime: %f + %f = %f\n", valeur_a_gauche, valeur_a_droite, resultat);

        /* Continuer avec le résultat (associativité à gauche) */
        resultat = parseE_prime(resultat);
    }

    // Production: E' → - T E'
    else if (t->type == MINUS) {
        DEBUG_MSG("parseE_prime: Operation MINUS detectee\n");
        consume(MINUS);

        float valeur_a_droite = parseT();
        DEBUG_VAL("parseE_prime: Operande droite", valeur_a_droite);

        resultat = valeur_a_gauche - valeur_a_droite;
        DEBUG_MSG("parseE_prime: %f - %f = %f\n", valeur_a_gauche, valeur_a_droite, resultat);

        resultat = parseE_prime(resultat);
    }
    // Production : E' → ε (pas d'opérateur, on s'arrête)
    else {
        DEBUG_MSG("parseE_prime: Production vide (epsilon)\n");
        resultat = valeur_a_gauche;
    }

    DEBUG_LEAVE("parseE_prime");
    VERIFIER_SUIVANTS('e', currentToken());
    return resultat;
}



//  T  → U T'
float parseT() {
    DEBUG_ENTER("parseT");

    /* Évaluer le premier facteur unaire */
    float resultat = parseU();
    DEBUG_VAL("parseT: Premier facteur", resultat);

    /* Traiter les multiplications/divisions restantes */
    resultat = parseT_prime(resultat);
    DEBUG_VAL("parseT: resultat final", resultat);

    DEBUG_LEAVE("parseT");
    VERIFIER_SUIVANTS('T', currentToken());
    return resultat;
}



//  T' → * U T' | / U T' | ε
float parseT_prime(float valeur_a_gauche) {
    DEBUG_ENTER("parseT_prime");
    DEBUG_VAL("valeur_a_gauche", valeur_a_gauche);

    if (currentToken() == NULL) {
        DEBUG_MSG("parseT_prime: Fin de fichier (epsilon)\n");
        DEBUG_LEAVE("parseT_prime");
        VERIFIER_SUIVANTS('t', currentToken());
        return valeur_a_gauche;
    }

    Token *t = currentToken();
    float resultat = valeur_a_gauche;

    // Production: T' → * U T'
    if (t->type == MULT) {
        DEBUG_MSG("parseT_prime: Operation MULT detectee\n");
        consume(MULT);

        float valeur_a_droite = parseU();
        DEBUG_VAL("parseT_prime: Operande droite", valeur_a_droite);

        resultat = valeur_a_gauche * valeur_a_droite;
        DEBUG_MSG("parseT_prime: %f * %f = %f\n", valeur_a_gauche, valeur_a_droite, resultat);

        resultat = parseT_prime(resultat);
    }
    // Production: T' → / U T'
    else if (t->type == DIV) {
        DEBUG_MSG("parseT_prime: Operation DIV detectee\n");
        consume(DIV);

        float valeur_a_droite = parseU();
        DEBUG_VAL("parseT_prime: Operande droite", valeur_a_droite);

        /* Vérification de la division par zéro */
        if (valeur_a_droite == 0.0f) {
            DEBUG_MSG("!!! ERREUR !!! Division par zero\n");
            fprintf(stdout, "!!! ERREUR !!!\n");
            exit(1);
        }

        resultat = valeur_a_gauche / valeur_a_droite;
        DEBUG_MSG("parseT_prime: %f / %f = %f\n", valeur_a_gauche, valeur_a_droite, resultat);

        resultat = parseT_prime(resultat);
    }
    /**
    // Production: T' → % U T'
    else if (t->type == MODULO) {
        DEBUG_MSG("parseT_prime: Operation MODULO detectee\n");
        consume(MODULO);

        float valeur_a_droite = parseU();
        DEBUG_VAL("parseT_prime: Operande droite", valeur_a_droite);

        if (valeur_a_droite == 0.0f) {
            DEBUG_MSG("!!! ERREUR !!! Modulo par zero\n");
            fprintf(stdout, "!!! ERREUR !!!\n");
            exit(1);
        }

        resultat = fmodf(valeur_a_gauche, valeur_a_droite);
        DEBUG_MSG("parseT_prime: %f %% %f = %f\n", valeur_a_gauche, valeur_a_droite, resultat);

        resultat = parseT_prime(resultat);
    }
    */
    // Production: T' → ε
    else {
        DEBUG_MSG("parseT_prime: Production vide (epsilon)\n");
        resultat = valeur_a_gauche;
    }

    DEBUG_LEAVE("parseT_prime");
    VERIFIER_SUIVANTS('t', currentToken());
    return resultat;
}



//  U  → P U'
float parseU() {
    DEBUG_ENTER("parseU");

    /* Évaluer le facteur primaire */
    float resultat = parseP();
    DEBUG_VAL("parseU: Facteur primaire", resultat);

    /* Traiter les puissances */
    resultat = parseU_prime(resultat);
    DEBUG_VAL("parseU: resultat final", resultat);

    DEBUG_LEAVE("parseU");
    VERIFIER_SUIVANTS('U', currentToken());
    return resultat;
}



/* U' → ^ P U' | ε
   Note : implémentation pour gérer l'associativité à droite des puissances */
float parseU_prime(float valeur_a_gauche) {
    DEBUG_ENTER("parseU_prime");
    DEBUG_VAL("valeur_a_gauche", valeur_a_gauche);

    if (currentToken() == NULL) {
        DEBUG_MSG("parseU_prime: Fin de fichier (epsilon)\n");
        DEBUG_LEAVE("parseU_prime");
        VERIFIER_SUIVANTS('u', currentToken());
        return valeur_a_gauche;
    }

    Token *t = currentToken();
    float resultat = valeur_a_gauche;

    // Production: U' → ^ P U'
    if (t->type == POWER) {
        DEBUG_MSG("parseU_prime: Operation POWER detectee\n");
        consume(POWER);

        // Évaluer la base de la puissance suivante
        float base_valeur_a_droite = parseP();
        DEBUG_VAL("parseU_prime: Base droite", base_valeur_a_droite);

        // Traiter les puissances restantes (associativité à droite)
        float valeur_a_droite = parseU_prime(base_valeur_a_droite);
        DEBUG_VAL("parseU_prime: Valeur droite complete", valeur_a_droite);

        // Calculer la puissance
        resultat = powf(valeur_a_gauche, valeur_a_droite);
        DEBUG_MSG("parseU_prime: %f ^ %f = %f\n", valeur_a_gauche, valeur_a_droite, resultat);
        VERIFIER_SUIVANTS('u', currentToken());
        return resultat;
    }
    // Production: U' → ε
    else {
        DEBUG_MSG("parseU_prime: Production vide (epsilon)\n");
        resultat = valeur_a_gauche;
    }

    DEBUG_LEAVE("parseU_prime");
    VERIFIER_SUIVANTS('u', currentToken());
    return resultat;
}



//  P  → cos A | sin A | F
float parseP() {
    DEBUG_ENTER("parseP");

    Token *t = currentToken();
    float resultat = 0.0f;

    /**
    // Production : P → + P
    if (t != NULL && t->type == PLUS) {
        DEBUG_MSG("parseP: Operateur unaire PLUS detecte\n");
        consume(PLUS);
        resultat = parseP();
        DEBUG_VAL("parseP: Resultat unaire PLUS", resultat);
    }

    // Production : P → - P
    else if (t != NULL && t->type == MINUS) {
        DEBUG_MSG("parseP: Operateur unaire MINUS detecte\n");
        consume(MINUS);
        resultat = -parseP();
        DEBUG_VAL("parseP: Resultat unaire MINUS", resultat);
    }
    */
    // Production: P → sin A
    if (t != NULL && t->type == SINE) {
        DEBUG_MSG("parseP: Operateur SINE detecte\n");
        consume(SINE);

        // ATTENTION : On appelle parseP() récursivement, pas parseF()
        // Cela permet à "sin x + 2" d'être interprété comme "sin(x + 2)"
        float arg = parseA();
        DEBUG_VAL("parseP: Argument de sin", arg);

        resultat = sinf(arg);
        DEBUG_MSG("parseP: sin(%f) = %f\n", arg, resultat);
    }

    // Production : P → cos A
    else if (t != NULL && t->type == COSINE) {
        DEBUG_MSG("parseP: Operateur COSINE detecte\n");
        consume(COSINE);

        float arg = parseA();
        DEBUG_VAL("parseP: Argument de cos", arg);

        resultat = cosf(arg);
        DEBUG_MSG("parseP: cos(%f) = %f\n", arg, resultat);
    }

    // Production: P → F
    else {
        DEBUG_MSG("parseP: Delegation a parseF\n");
        resultat =  parseF();
    }

    DEBUG_LEAVE("parseP");
    VERIFIER_SUIVANTS('P', currentToken());
    return resultat;
}



//  F  → x | immediate | pi | (A)
float parseF() {
    DEBUG_ENTER("parseF");

    Token *t = currentToken();
    float resultat = 0.0f;

    // Production: F → x (variable)
    if (t != NULL && t->type == VAR) {
        // Sauvegarder le nom avant de consommer
        char *nom_variable = get_text(t);
        DEBUG_MSG("parseF: Variable %s\n", nom_variable);

        consume(VAR);

        // Récupérer la valeur de la variable
        resultat = get_value(nom_variable);
        DEBUG_VAL("parseF: Valeur", resultat);

        free(nom_variable);
    }

    // Production : F → immediate (nombre littéral)
    else if (t != NULL && t->type == IMMEDIATE) {
        float val = t->value;
        DEBUG_MSG("parseF: Immediate %f\n", val);

        consume(IMMEDIATE);
        resultat =  val;
    }

    // Production : F → pi (constante pi)
    else if (t != NULL && t->type == PI) {
        DEBUG_MSG("parseF: Constante PI\n");
        consume(PI);
#ifdef M_PI
        resultat = (float)M_PI;  /* M_PI est défini dans math.h sur la plupart des plateformes */
#else
        resultat = 3.14159265358979323846f;
#endif
    }

    // Production: F → ( A )
    else if (t != NULL && t->type == OPAR) {
        DEBUG_MSG("parseF: Parenthese ouvrante\n");
        consume(OPAR);

        // Analyser l'assignation à l'intérieur des parenthèses
        resultat = parseA();
        DEBUG_VAL("parseF: Expression entre parentheses", resultat);

        consume(FPAR);
        DEBUG_MSG("parseF: Parenthese fermante\n");
    }

    // Token inattendu = erreur
    else {
        DEBUG_MSG(RED "!!! ERREUR !!! " RESET
            "Token inattendu dans parseF : " RED "\"%s\" " RESET
            "à la place de "
            GREEN "\"x\"" RESET ", "
            GREEN "\"immediate\"" RESET ", "
            GREEN "\"pi\"" RESET ", "
            GREEN "\"(\"" RESET
            "\n",
            t->text
        );
        fprintf(stdout, "!!! ERREUR !!!\n");
        exit(1);
    }

    DEBUG_LEAVE("parseF");
    VERIFIER_SUIVANTS('F', currentToken());
    return resultat;
}




/*********** PROGRAMME PRINCIPAL ************/
/**
 * Point d'entrée du programme.
 *
 * Usage : ./assignations fichier.txt
 *
 * Le programme :
 * 1. Initialise le lexer avec le fichier d'entrée
 * 2. Lance l'analyse syntaxique avec parseS()
 * 3. Vérifie qu'il n'y a pas de tokens restants (erreur sinon)
 * 4. Affiche la table des symboles
 *
 * @param argc Nombre d'arguments
 * @param argv Arguments de la ligne de commande
 * @return 0 si succès, 1 si erreur
 */
int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s fichier.txt\n", argv[0]);
        return 1;
    }

    DEBUG_MSG("=== Debut de l'analyse ===\n");
    DEBUG_MSG("Fichier d'entree: %s\n", argv[1]);

    // Initialisation de l'analyseur lexical
    initLexer(argv[1]);

    // APPEL A LA SOURCE DE LA GRAMMAIRE (parseS)
    DEBUG_MSG("\n=== Lancement de SOURCE_GRAMMAIRE() ===\n");
    SOURCE_GRAMMAIRE();
    DEBUG_MSG("=== Fin de SOURCE_GRAMMAIRE() ===\n\n");

    // ON VERIFIE QUE LA GRAMMAIRE A BIEN TERMINE SON TRAVAIL A LA FIN DU MOT A ANALYSER
    if (currentToken() != NULL) {
        DEBUG_MSG("!!! ERREUR !!! Tokens inattendus apres la fin de l'analyse.\n");
        Token *rt = currentToken();
        DEBUG_MSG("Token restant: type %d, texte '%s'\n", rt->type, rt->text ? rt->text : "(NULL)");
        fprintf(stdout, "!!! ERREUR !!!\n");
        return 1;
    }

    DEBUG_MSG("\n=== Analyse terminee avec succes ===\n");
    DEBUG_MSG("=== Affichage de la table des symboles ===\n\n");

    // ET ON AFFICHE LA TABLE DES SYMBOLES
    print_symbols();
    return 0;
}

