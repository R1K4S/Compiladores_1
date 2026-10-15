%{
#include "parser.tab.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
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

[a-zA-Z_][a-zA-Z0-9_]*              { yylval.sval = strdup(yytext); return ID; }

[0-9]+\.[0-9]+                      {
                                        yylval.fval = atof(yytext);
                                        return NUM_FLOAT;
                                    }
[0-9]+                              {
                                        yylval.ival = atoi(yytext);
                                        return NUM;
                                    }
'([^'\\\n]|\\.)'                    { yylval.cval = yytext[1]; return CHAR_LITERAL; }
\"([^\"\\\n]|\\.)*\"                { yylval.sval = strdup(yytext); return STRING_LITERAL; }

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
