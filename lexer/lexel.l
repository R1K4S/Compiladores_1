%{
#include "calc.tab.h"   /* Para Bison: #include do arquivo gerado, contendo tokens */
#include <stdlib.h>
%}

%%

[ \t\n]+              /* ignora espaços */
[0-9]+  {
    yylval.intValue = atoi(yytext);
    return NUM;
}
"+"     { return PLUS; }
"-"     { return MINUS; }
"*"     { return TIMES; }
"/"     { return DIV; }
"("     { return LPAREN; }
")"     { return RPAREN; }
.       { /* tudo mais é ignorado ou tratável como erro */ }

%%
int yywrap() { return 1; }
