/*
 * expr.lex : Petit scanner pour un parseur d'expressions basique.
 */

%{
#include "expr.h"
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#define YY_SKIP_YYWRAP 1

int yywrap() {
    return 1;
}

%}

%%

[0-9]+          { yylval.valeur_flottante = atof(yytext); return(FLOTTANT); }
[0-9]+\.[0-9]+  { yylval.valeur_flottante = atof(yytext); return(FLOTTANT); }

[a-zA-Z]+       { yylval.texte = strdup(yytext); return(VAR); }

"+"        return(PLUS);
"*"        return(MULT);
"("        return(LB);
")"        return(RB);
[\n\t ]*     /* on ignore les espaces et tabulations */

<<EOF>>  { return 0; }
%%

