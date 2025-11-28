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

/* Priorités de plus faible (ASSIGN) à plus forte (UMINUS). */
%right ASSIGN
%right COS SIN
%left PLUS MINUS
%left MULT DIV MODULO
%right EXPON
%precedence UPLUS UMINUS

%type<valeur_flottante> Programme ListeInstr Instruction ExprAffectation Expression

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

/* Grammaire ambiguë résolue par les directives de priorité ci-dessus. */
Expression:
      Expression PLUS Expression     { $$ = $1 + $3; }
    | Expression MINUS Expression    { $$ = $1 - $3; }
    | Expression MULT Expression     { $$ = $1 * $3; }
    | Expression DIV Expression      { $$ = $1 / $3; }
    | Expression MODULO Expression   { $$ = fmodf($1, $3); }
    | Expression EXPON Expression    { $$ = powf($1, $3); }
    | COS Expression %prec COS { $$ = cosf($2); }
    | SIN Expression %prec SIN { $$ = sinf($2); }
    | PLUS Expression %prec UPLUS { $$ = $2; }
    | MINUS Expression %prec UMINUS { $$ = -$2; }
    | LB ExprAffectation RB   { $$ = $2; }
    | FLOTTANT           { $$ = $1; }
    | PI                 { $$ = $1; }
    | VAR                { $$ = get_value($1); free($1); }
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
