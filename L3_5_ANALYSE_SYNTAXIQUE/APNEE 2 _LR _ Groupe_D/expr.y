%{
#include <stdio.h>
int yyparse();
int yylex();
int yyerror(char *message);
%}

// Petit rappel : Bison récupère les terminaux via yylex() et passe les valeurs
// au parser via la variable yylval. On déclare ici un union C pour indiquer
// quels types peuvent circuler (float pour les nombres, char* pour un nom).
%union {
  float valeur_flottante;
  char *texte;
}

// Symboles terminaux fournis par yylex(), en majuscules pour suivre la convention.
%token FLOTTANT VAR PLUS MULT LB RB

// Typage des terminaux et non-terminaux qui portent des valeurs.
%type<valeur_flottante> Expression Terme Facteur FLOTTANT
%type<texte> VAR

%%

Programme: Expression { printf("Resultat: %f\n", $1); }

// Règles E -> E + T | T
Expression: Expression PLUS Terme { $$ = $1 + $3 ; }
    | Terme { $$ = $1; }

Terme : Terme MULT Facteur { $$ = $1 * $3; }
    | Facteur { $$ = $1; }

Facteur : FLOTTANT     { $$ = $1; }
    | VAR           { fprintf(stderr, "Variable non traitee ici, %s renvoie 0\n", $1); $$ = 0; }
    | LB Expression RB    { $$ = $2; }

%%

int yyerror(char *message) {
    fprintf(stderr, "Erreur: %s\n", message);
    fprintf(stdout, "!!! ERREUR !!!\n");
    return 0;
}

int main(void) {
    yyparse();
    return 0;
}
