%{
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "table_symboles.h"
int yyparse();
int yylex();
int yyerror(char *message);
%}

%union {
  float valeur_flottante;
  char *texte;
}

%token<valeur_flottante> FLOTTANT PI
%token<texte> VAR
%token PLUS MINUS MULT DIV EXPON SEMICOLON LB RB MODULO ASSIGN INTERO COLON COS SIN

%type<valeur_flottante> Programme ListeInstr Instruction ExprAffectation Expression ExpressionSomme Terme Puissance Unaire Primaire

%%

Programme: ListeInstr { print_symbols(); $$ = 0.0f; }

ListeInstr:
      /* vide */ { $$ = 0.0f; }
    | ListeInstr Instruction { $$ = $2; }
    ;

Instruction: ExprAffectation SEMICOLON { $$ = $1; }
    ;

ExprAffectation:
      VAR ASSIGN ExprAffectation { set_value($1, $3); $$ = $3; free($1); }
    | Expression { $$ = $1; }
    ;

/* cos/sin priorité la plus basse : on leur passe une expression complète. */
Expression:
      ExpressionSomme { $$ = $1; }
    ;

ExpressionSomme:
      ExpressionSomme PLUS Terme { $$ = $1 + $3; }
    | ExpressionSomme MINUS Terme { $$ = $1 - $3; }
    | Terme { $$ = $1; }
    ;

Terme:
      Terme MULT Puissance { $$ = $1 * $3; }
    | Terme DIV Puissance { $$ = $1 / $3; }
    | Terme MODULO Puissance { $$ = fmodf($1, $3); }
    | Puissance { $$ = $1; }
    ;

/* Exponentiation droite-associative. */
Puissance:
      Unaire EXPON Puissance { $$ = powf($1, $3); }
    | Unaire { $$ = $1; }
    ;

Unaire:
      MINUS Unaire { $$ = -$2; }
    | PLUS Unaire { $$ = $2; }
    | Primaire { $$ = $1; }
    ;

Primaire:
      COS ExpressionSomme { $$ = cosf($2); }
    | SIN ExpressionSomme { $$ = sinf($2); }
    | FLOTTANT { $$ = $1; }
    | PI { $$ = $1; }
    | VAR { $$ = get_value($1); free($1); }
    | LB ExprAffectation RB { $$ = $2; }
    ;

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
