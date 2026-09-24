%{
#include "parser.tab.h"
%}

%%

"//".*                          { /* Ignorar os comentarios de uma linha */ }
"/*"([^*]|\*+[^*/])*\*+"/"      { /* Ignorar os comentarios multilinha */ }

"if"            { return IF; }
"else"          { return ELSE; }
"while"         { return WHILE; }
"{"             { return LBRACE; }
"}"             { return RBRACE; }
"int"           { return INT; }
"float"         { return FLOAT; }
"char"          { return CHAR; }
"if"            { return IF; }
"else"          { return ELSE; }
"while"         { return WHILE; }
"return"        { return RETURN; }

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
