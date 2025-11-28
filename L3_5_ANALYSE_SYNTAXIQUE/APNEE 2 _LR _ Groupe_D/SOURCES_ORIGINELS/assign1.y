%{
#include <stdio.h>
#include "table_symboles.h"
int yyparse();
int yylex();
int yyerror(char *s);
%}

%union {
  float fval;
  char *sval;
}

%token FLOTTANT VAR PLUS MINUS MULT DIV EXPON SEMICOLON LB RB MODULO ASSIGN INTERO COLON

%type<fval> Expr FLOTTANT
%type<sval> VAR

%%

Source: Expr { print_symbols(); }

Expr: { $$ = 0.0 ; }

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