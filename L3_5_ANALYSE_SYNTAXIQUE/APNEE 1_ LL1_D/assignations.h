//
// Created by Gilles Sérasset on 09/10/2019.
//

#ifndef SIMPLE_ASSIGN_ASSIGNATIONS_H
#define SIMPLE_ASSIGN_ASSIGNATIONS_H


/**
 * GRAMMAIRE COMPLETE LL2 pour les assignations :
 *
 *  N = { S', S, A, E, E', T, T', U, U', P, F }
 *  T = { ;, x, =, +, -, *, /, ^, cos, sin, immediate, pi, (, ) }
 *
 *  S' → S $
 *  S  → A ; S | ε
 *  A  → x = A | E
 *  E  → T E'
 *  E' → + T E' | - T E' | ε
 *  T  → U T'
 *  T' → * U T' | / U T' | ε
 *  U  → P U'
 *  U' → ^ P U' | ε
 *  P  → cos A | sin A | F
 *  F  → x | immediate | pi | (A)
 *
 *
 * où :
 * - '$' représente la fin du fichier
 * - 'x' représente une variable (token VAR)
 * - 'immediate' représente un nombre (token IMMEDIATE)
 * - Les opérateurs 'cos' et 'sin' ont une priorité inférieure à l'addition dans ce cas-ci
 *   (donc "cos x + 2" s'évalue comme "cos(x + 2)")
 *
 * PRIORITES DES OPERATEURS (du plus bas au plus haut) :
 * 1. Assignation (=)           - Associatif à droite
 * 2. Addition/Soustraction     - Associatif à gauche (grâce au chaînage)
 * 3. Multiplication/Division   - Associatif à gauche
 * 4. Puissance (^)             - Associatif à droite
 * 5. Opérateurs unaires (sin, cos)
 * 6. Facteurs (variables, constantes, parenthèses)
 */




/**
 * SOURCE_GRAMMAIRE : Analyse toutes les séquences d'assignation
 *
 * Règle : S' → S $
 *
 * Cette fonction traite toutes les séquences d'assignations dans le fichier en entrée jusqu'à la fin du fichier
*/
void SOURCE_GRAMMAIRE();



/**
 * parseS : Analyse une séquence d'assignations
 *
 * Règle : S → A ; S | ε
 *
 * Cette fonction traite zéro ou plusieurs assignations séparées par des point virgules.
 * Elle continue jusqu'à ce qu'il n'y ait plus de tokens.
 *
 * @return La valeur de la dernière assignation (ou 0 si aucune)
 */
float parseS();



/**
 * parseA : Analyse une assignation
 *
 * Règle : A → x = A | E
 *
 * Cette fonction gère deux cas :
 * 1. x = A : Une assignation (avec associativité à droite pour les chaînes)
 * 2. E : Une simple expression (sans assignation)
 *
 * Pour distinguer les deux cas, on regarde 2 tokens à l'avance (LL2) :
 * - Si on a VAR suivi de ASSIGN, c'est une assignation
 * - Sinon, c'est une expression
 *
 * @return La valeur de l'assignation ou de l'expression
 */
float parseA() ;



/**
 * parseE : Analyse une expression (addition/soustraction)
 *
 * Règle : E → T E'
 *
 * Cette fonction traite les additions et soustractions.
 * Elle évalue d'abord un terme, puis traite les opérations restantes.
 *
 * @return La valeur de l'expression
 */
float parseE();



/**
 * parseE_prime : Traite les additions/soustractions restantes
 *
 * Règle : E' → + T E' | - T E' | ε
 *
 * Cette fonction utilise le "chaînage des résultats" pour obtenir
 * l'associativité à gauche malgré la récursion à droite.
 *
 * Exemple : Pour "10 - 5 - 2"
 * 1. parseE_prime(10) voit '-', calcule 10 - 5 = 5
 * 2. parseE_prime(5) voit '-', calcule 5 - 2 = 3
 * 3. parseE_prime(3) ne voit plus d'opérateur, retourne 3
 *
 * Résultat : (10 - 5) - 2 = 3 (associativité à gauche)
 *
 * @param valeur_a_gauche Valeur accumulée jusqu'ici
 * @return La valeur finale après toutes les additions/soustractions
 */
float parseE_prime(float valeur_a_gauche);



/**
 * parseT : Analyse un terme (multiplication/division)
 *
 * Règle : T → U T'
 *
 * @return La valeur du terme
 */
float parseT() ;



/**
 * parseT_prime : Traite les multiplications/divisions restantes
 *
 * Règle : T' → * U T' | / U T' | ε
 *
 * Utilise le même principe de chaînage que parseE_prime
 * pour obtenir l'associativité à gauche.
 *
 * @param valeur_a_gauche Valeur accumulée jusqu'ici
 * @return La valeur finale après toutes les multiplications/divisions
 */
float parseT_prime(float valeur_a_gauche);



/**
 * parseU : Analyse un facteur avec puissance
 *
 * Règle : U → P U'
 *
 * @return La valeur du facteur avec puissance
 */
float parseU();




/**
 * parseU_prime : Traite les puissances
 *
 * Règle : U' → ^ P U' | ε
 *
 * NOTE IMPORTANTE : La puissance est Associative à DROITE.
 * Exemple : 2^3^2 = 2^(3^2) = 2^9 = 512 (et non (2^3)^2 = 64)
 *
 * Pour cela, on évalue d'abord la partie droite complètement
 * avant de calculer la puissance.
 *
 * @param left_value Base de la puissance
 * @return Le résultat après application des puissances
 */
float parseU_prime(float left_value);



/**
 * parseP : Analyse un facteur primaire (avec opérateurs unaires)
 *
 * Règle : P → cos A | sin A | F
 *
 * Les opérateurs cos et sin ont une priorité INFERIEURE à l'addition dans ce cas,
 * donc "cos x + 2" s'évalue comme "cos(x + 2)" et non "cos(x) + 2".
 *
 * C'est pourquoi parseP() appelle parseA() après avoir rencontré sin ou cos,
 * ce qui permet d'englober toute l'expression.
 *
 * @return La valeur du facteur primaire
 */
float parseP();



/**
 * parseF : Analyse un facteur de base
 *
 * Règle : F → x | immediate | pi | ( A )
 *
 * Cette fonction traite les éléments de base :
 * - Variables (VAR)
 * - Nombres littéraux (IMMEDIATE)
 * - La constante pi (PI)
 * - Expressions entre parenthèses (OPAR A FPAR)
 *
 * @return La valeur du facteur
 */
float parseF();




/******************************************************************************
 * Le code ci-desous devait se trouver dans "assignations.c"
 * ou dans un autre fichier, il est mis ici dans le souci de ne pas parasiter
 * "assignations.c" avec trop de fonctions qui n'ont pas de lien direct avec
 * ce qui est demandé et de ne pas avoir plusieurs fichiers inutils
 * ***************************************************************************/


/************* DEBUG TREE SYSTEM *************/
#ifdef DEBUG
#undef DEBUG
#define DEBUG 1
#else
#define DEBUG 0
#endif

#define RED     "\033[1;31m"
#define GREEN   "\033[1;32m"
#define RESET   "\033[0m"


static int DEBUG_INDENT = 0;

void debug_indent() {
 for (int i = 0; i < DEBUG_INDENT; i++)
  fprintf(stderr, "│  ");
}

void DEBUG_ENTER(const char *func) {
 if (!DEBUG) return;
 debug_indent();
 fprintf(stderr, "├─ APPEL : %s()\n", func);
 DEBUG_INDENT++;
}

void DEBUG_LEAVE(const char *func) {
 if (!DEBUG) return;
 DEBUG_INDENT--;
 debug_indent();
 fprintf(stderr, "└─ RETOUR de %s()\n", func);
}

void DEBUG_MSG(const char *fmt, ...) {
 if (!DEBUG) return;
 debug_indent();
 fprintf(stderr, "│  ");
 va_list ap;
 va_start(ap, fmt);
 vfprintf(stderr, fmt, ap);
 va_end(ap);
}

void DEBUG_VAL(const char *name, float val) {
 if (!DEBUG) return;
 debug_indent();
 fprintf(stderr, "│  %s = %f\n", name, val);
}

void RESET_INDENT() {
 DEBUG_INDENT = 0;
 fprintf(stderr, "\n\n");
}



void VERIFIER_SUIVANTS(char non_terminal, Token *courant) {
  switch (non_terminal) {
    case 'S': // SUIVANT(S) = { $ }
      if (courant != NULL) {
        DEBUG_MSG(RED "!!! ERREUR !!! " RESET "Toutes les expressions devaient déjà être évaluées\n");
        fprintf(stdout, "!!! ERREUR !!!\n");
        exit(1);
      }
      break;

    case 'E':  // SUIVANT(E) = { ;, ) }
    case 'e':  // SUIVANT(E') = { ;, ) }  (utiliser 'e' pour E')
      if (courant == NULL) {
        DEBUG_MSG(RED "!!! ERREUR !!! " RESET "Token attendu après expression (';' ou ')')\n");
        fprintf(stdout, "!!! ERREUR !!!\n");
        exit(1);
      }
      if (courant->type != SEMICOLON && courant->type != FPAR) {
        DEBUG_MSG(
            RED "!!! ERREUR !!! " RESET
            "Token inattendu après expression E : " RED "\"%s\" " RESET
            "attendu : " GREEN "\";\" " RESET "ou " GREEN "\")\"" RESET "\n",
            courant->text
        );
        fprintf(stdout, "!!! ERREUR !!!\n");
        exit(1);
      }
      break;

    case 'T':  // SUIVANT(T) = { ;, ), +, - }
    case 't':  // SUIVANT(T') = { ;, ), +, - } (utiliser 't' pour T')
      if (courant == NULL) {
        DEBUG_MSG(RED "!!! ERREUR !!! " RESET "Token attendu après terme\n");
        fprintf(stdout, "!!! ERREUR !!!\n");
        exit(1);
      }
      if (courant->type != SEMICOLON && courant->type != FPAR &&
          courant->type != PLUS && courant->type != MINUS) {
        DEBUG_MSG(
            RED "!!! ERREUR !!! " RESET
            "Token inattendu après terme T : " RED "\"%s\" " RESET
            "attendu : " GREEN "\";\" " RESET ", " GREEN "\")\" " RESET
            ", " GREEN "\"+\" " RESET "ou " GREEN "\"-\"" RESET "\n",
            courant->text
        );
        fprintf(stdout, "!!! ERREUR !!!\n");
        exit(1);
      }
      break;

    case 'U':  // SUIVANT(U) = { ;, ), +, -, *, / }  ou avec "%"
    case 'u':  // SUIVANT(U') = { ;, ), +, -, *, / } ou avec "%"  (utiliser 'u' pour U')
      if (courant == NULL) {
        DEBUG_MSG(RED "!!! ERREUR !!! " RESET "Token attendu après unaire\n");
        fprintf(stdout, "!!! ERREUR !!!\n");
        exit(1);
      }
      if (courant->type != SEMICOLON && courant->type != FPAR &&
          courant->type != PLUS && courant->type != MINUS &&
          courant->type != MULT && courant->type != DIV 
          // && courant->type != MODULO
          ) {
        DEBUG_MSG(
            RED "!!! ERREUR !!! " RESET
            "Token inattendu après unaire U : " RED "\"%s\" " RESET
            "attendu : " GREEN "\";\" " RESET ", " GREEN "\")\" " RESET
            ", " GREEN "\"+\" " RESET ", " GREEN "\"-\" " RESET
            ", " GREEN "\"*\" " RESET ", " GREEN "\"/\" " RESET
            //"ou " GREEN "\"%\"" RESET "\n"
            ,
            courant->text
        );
        fprintf(stdout, "!!! ERREUR !!!\n");
        exit(1);
      }
      break;

    case 'A':  // SUIVANT(A) = { ;, ), +, -, *, /, ^ } ou avec "%"
    case 'P':  // SUIVANT(P) = { ;, ), +, -, *, /, ^ } ou avec "%"
    case 'F':  // SUIVANT(F) = { ;, ), +, -, *, /, ^ } ou avec "%"
      if (courant == NULL) {
        DEBUG_MSG(RED "!!! ERREUR !!! " RESET "Token attendu après facteur\n");
        fprintf(stdout, "!!! ERREUR !!!\n");
        exit(1);
      }
      if (courant->type != SEMICOLON && courant->type != FPAR &&
          courant->type != PLUS && courant->type != MINUS &&
          courant->type != MULT && courant->type != DIV &&
          courant->type != POWER 
          //&& courant->type != MODULO
          ) {
        DEBUG_MSG(
            RED "!!! ERREUR !!! " RESET
            "Token inattendu après facteur P/F : " RED "\"%s\" " RESET
            "attendu : " GREEN "\";\" " RESET ", " GREEN "\")\" " RESET
            ", " GREEN "\"+\" " RESET ", " GREEN "\"-\" " RESET
            ", " GREEN "\"*\" " RESET ", " GREEN "\"/\" " RESET
            //", " GREEN "\"%\" " RESET 
            "ou " GREEN "\"^\"" RESET "\n",
            courant->text
        );
        fprintf(stdout, "!!! ERREUR !!!\n");
        exit(1);
      }
      break;

    default:
      DEBUG_MSG(RED "!!! ERREUR !!! " RESET "Non-terminal inconnu : '%c'\n", non_terminal);
      fprintf(stdout, "!!! ERREUR !!!\n");
      exit(1);
  }
}




#endif //SIMPLE_ASSIGN_ASSIGNATIONS_H
