%{
#include "parser.tab.h"
%}

%%

"//".*                          { /* Ignorar comentarios de uma linha */ }
"/*"([^*]|\*+[^*/])*\*+"/"      { /* Ignorar comentarios multilinha */ }

[0-9]+          { yylval = atoi(yytext); return NUM; }
"+"             { return PLUS; }
"-"             { return MINUS; }
"*"             { return TIMES; }
"/"             { return DIVIDE; }
"("             { return LPAREN; }
")"             { return RPAREN; }
[ \t\n\r]+      { /* Ignorar espacos em branco, tabs e quebras de linha */ }
.               { printf("Caractere invalido: %s\n", yytext); }

%%
int yywrap(void) { return 1; }
