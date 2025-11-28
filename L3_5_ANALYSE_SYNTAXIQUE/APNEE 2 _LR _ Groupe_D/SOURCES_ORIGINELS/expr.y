%{
#include <stdio.h>
int yyparse();
int yylex();
int yyerror(char *s);
%}

// Bison va demander à flex de récupérer le prochain terminal, qui est retourné
// en utilisant le type "yystype".  Par défaut, yystype est just un int, mais pour les
// projets plus complexes, les terminaux peuvent porter des informations de types plus
// complexes. Pour cela, on utilise un union C comme type. En bison on déclare les différents
// types de l'union dans la directive %union.
%union {
  float fval;
  char *sval;
}

// Symboles terminaux qui seront fournis par yylex(). La convention en bison est d'utiliser
// des constantes en majuscules comme nom pour les terminaux.
%token FLOTTANT VAR PLUS MULT LB RB

// Et lorsqu'on utilise un type union, il faut aussi déclarer le type des valeurs associées aux différents
// terminaux et non terminaux.
%type<fval> Expr Term Factor FLOTTANT
%type<sval> VAR

%%

Source: Expr { printf("Resultat: %f\n", $1); }

// Règles E -> E + T | T
Expr: Expr PLUS Term { $$ = $1 + $3 ; }
    | Term { $$ = $1; }

Term : Term MULT Factor { $$ = $1 * $3; }
    | Factor { $$ = $1; }

Factor : FLOTTANT     { $$ = $1; }
    | VAR           { fprintf(stderr, "Variables non encore traitees, %s renvoie 0\n", $1); $$ = 0; }
    | LB Expr RB    { $$ = $2; }

%%

int yyerror(char *s) {
    fprintf(stderr, "Erreur: %s\n", s);
    fprintf(stdout, "!!! ERREUR !!!\n");
    return 0;
}

int main(void) {
    yyparse();
    return 0;
}