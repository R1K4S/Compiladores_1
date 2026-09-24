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

[0-9]+          { yylval = atoi(yytext); return NUM; }

    /* Operadores de dois caracteres */
"=="            { return EQ; }
"!="            { return NEQ; }
"<="            { return LE; }
">="            { return GE; }
"&&"            { return AND; }
"||"            { return OR; }

    /* Relacionais de um caractere, lógico não e atribuição */
"<"             { return LT; }
">"             { return GT; }
"!"             { return NOT; }
"="             { return ASSIGN; }

    /* Aritméticos */
"+"             { return PLUS; }
"-"             { return MINUS; }
"*"             { return TIMES; }
"/"             { return DIVIDE; }
"%"             { return MOD; }

    /* Pontuação (LBRACE/RBRACE já tratados acima, junto das palavras-chave) */
"("             { return LPAREN; }
")"             { return RPAREN; }
";"             { return SEMI; }
","             { return COMMA; }

[ \t\n\r]+      { /* Ignorar espacos em branco, tabs e quebras de linha */ }
.               { printf("Caractere invalido: %s\n", yytext); }

%%
int yywrap(void) { return 1; }
