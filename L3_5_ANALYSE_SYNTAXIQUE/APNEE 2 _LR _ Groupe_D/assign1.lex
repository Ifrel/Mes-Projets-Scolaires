/*
 * assign1.lex : petit scanner pour parser les affectations avec priorités.
 */

%{
#include "assign1.h"
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define YY_SKIP_YYWRAP 1

int yywrap() {
    return 1;
}

%}

%%

[0-9]+          { yylval.valeur_flottante = atof(yytext); return(FLOTTANT); }
[0-9]+\.[0-9]+  { yylval.valeur_flottante = atof(yytext); return(FLOTTANT); }

[cC][oO][sS]    { return(COS); }
[sS][iI][nN]    { return(SIN); }
[pP][iI]        { yylval.valeur_flottante = (float)M_PI; return(PI); }

[a-zA-Z][a-zA-Z0-9_]*       { yylval.texte = strdup(yytext); return(VAR); }

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
[\n\t ]*     /* on ignore les espaces et tabulations */
.          { // chaque caractère inconnu devient un point-virgule...
             return(SEMICOLON);
           }
<<EOF>>  { return 0; }
%%
