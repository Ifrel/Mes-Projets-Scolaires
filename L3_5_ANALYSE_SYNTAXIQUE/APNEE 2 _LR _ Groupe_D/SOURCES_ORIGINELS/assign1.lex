/*
 * assign.lex : Scanner pour analyse des expressions.
 */

%{
#include "assign1.h"
#include <errno.h>

#define YY_SKIP_YYWRAP 1

int yywrap() {
    return 1;
}

%}

%%

[0-9]+          { yylval.fval = atof(yytext); return(FLOTTANT); }
[0-9]+\.[0-9]+  { yylval.fval = atof(yytext); return(FLOTTANT); }

[a-zA-Z]+       { yylval.sval = strdup(yytext); return(VAR); }

"+"        return(PLUS);
"-"        return(MINUS);
"*"        return(MULT);
"/"        return(DIV);
"^"        return(EXPON);
"%"        return(MODULO);
"("        return(LB);
")"        return(RB);
"="        return(ASSIGN);
";"         return(SEMICOLON);
"?"         return(INTERO);
":"         return(COLON);
[\n\t ]*     /* throw away whitespace */
.          { // Traiter chaque caractère inconnu comme un point virgule...
             return(SEMICOLON);
           }
<<EOF>>  { return(EOF); }
%%



