%{
#include "parser.tab.h"
#include <stdlib.h>
#include <stdio.h>
%}

%%

"//".*                              { /* Ignora comentario de uma linha */ }
"/*"([^*]|\*+[^*/])*\*+"/"          { /* Ignora comentario multilinha */ }

"if"                                { return IF; }
"else"                              { return ELSE; }
"while"                             { return WHILE; }

"int"                               { return INT; }
"float"                             { return FLOAT; }
"char"                              { return CHAR; }
"return"                            { return RETURN; }

[a-zA-Z_][a-zA-Z0-9_]*              { return ID; }

[0-9]+\.[0-9]+                      { return NUM_FLOAT; }
[0-9]+                              {
                                        yylval = atoi(yytext);
                                        return NUM;
                                    }
'([^'\\\n]|\\.)'                    { yylval = yytext[1]; return CHAR_LITERAL; }
\"([^\"\\\n]|\\.)*\"                { return STRING_LITERAL; }

"=="                                { return EQ; }
"!="                                { return NEQ; }
"<="                                { return LE; }
">="                                { return GE; }
"&&"                                { return AND; }
"||"                                { return OR; }

"<"                                 { return LT; }
">"                                 { return GT; }
"!"                                 { return NOT; }
"="                                 { return ASSIGN; }

"+"                                 { return PLUS; }
"-"                                 { return MINUS; }
"*"                                 { return TIMES; }
"/"                                 { return DIVIDE; }
"%"                                 { return MOD; }

"("                                 { return LPAREN; }
")"                                 { return RPAREN; }
"{"                                 { return LBRACE; }
"}"                                 { return RBRACE; }
";"                                 { return SEMI; }
","                                 { return COMMA; }

[ \t\n\r]+                          { /* Ignora espacos */ }

.                                   {
                                        fprintf(
                                            stderr,
                                            "Caractere invalido: %s\n",
                                            yytext
                                        );
                                    }

%%

int yywrap(void)
{
    return 1;
}