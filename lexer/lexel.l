%{
#include "parser.tab.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
%}

%option yylineno

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

[a-zA-Z_][a-zA-Z0-9_]*      { 
                                yylval.str_val = strdup(yytext); 
                                return ID; 
                            }

[0-9]+\.[0-9]+                      {
                                        yylval.float_val = atof(yytext);
                                        return NUM_FLOAT;
                                    }
[0-9]+                              {
                                        yylval.int_val = atoi(yytext);
                                        return NUM;
                                    }
'([^'\\\n]|\\.)'                    { yylval.int_val = yytext[1]; return CHAR_LITERAL; }
\"([^\"\\\n]|\\.)*\"                { yylval.str_val = strndup(yytext + 1, yyleng - 2); return STRING_LITERAL; }

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