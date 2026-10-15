%{
#include <stdio.h>
#include <stdlib.h>
#include "ast.h"

int yylex(void);
void yyerror(const char *s);
%}

%union {
    int ival;
    float fval;
    char cval;
    char *sval;
    struct NoAST *node;
}

%token <ival> NUM
%token <fval> NUM_FLOAT
%token <cval> CHAR_LITERAL
%token <sval> ID STRING_LITERAL

%token PLUS MINUS TIMES DIVIDE MOD

%token EQ NEQ LT GT LE GE

%token AND OR NOT

%token ASSIGN

%token LPAREN RPAREN
%token LBRACE RBRACE
%token SEMI COMMA

%token IF ELSE WHILE

%token INT FLOAT CHAR

%token RETURN

%type <node> expressao

%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

%right ASSIGN
%left OR
%left AND
%left EQ NEQ
%left LT GT LE GE
%left PLUS MINUS
%left TIMES DIVIDE MOD
%right NOT
%right UMINUS

%%

programa:
      lista_comandos
    ;

lista_comandos:
      lista_comandos comando
    | comando
    ;

comando:
      IF LPAREN expressao RPAREN comando %prec LOWER_THAN_ELSE
    | IF LPAREN expressao RPAREN comando ELSE comando
    | WHILE LPAREN expressao RPAREN comando
    | LBRACE lista_comandos RBRACE
    | declaracao
    | RETURN expressao SEMI
    | expressao SEMI
    ;

declaracao:
      tipo ID SEMI
    | tipo ID ASSIGN expressao SEMI
    | tipo lista_declaracoes SEMI
    ;

lista_declaracoes:
      declaracao_variavel
    | lista_declaracoes COMMA declaracao_variavel
    ;

declaracao_variavel:
      ID
    | ID ASSIGN expressao
    ;

tipo:
      INT
    | FLOAT
    | CHAR
    ;

expressao:
      expressao ASSIGN expressao     { $$ = criar_no_atribuicao($1, $3); }
    | expressao OR expressao         { $$ = criar_no_op_binaria(OR, $1, $3); }
    | expressao AND expressao        { $$ = criar_no_op_binaria(AND, $1, $3); }
    | expressao EQ expressao         { $$ = criar_no_op_binaria(EQ, $1, $3); }
    | expressao NEQ expressao        { $$ = criar_no_op_binaria(NEQ, $1, $3); }
    | expressao LT expressao         { $$ = criar_no_op_binaria(LT, $1, $3); }
    | expressao GT expressao         { $$ = criar_no_op_binaria(GT, $1, $3); }
    | expressao LE expressao         { $$ = criar_no_op_binaria(LE, $1, $3); }
    | expressao GE expressao         { $$ = criar_no_op_binaria(GE, $1, $3); }
    | expressao PLUS expressao       { $$ = criar_no_op_binaria(PLUS, $1, $3); }
    | expressao MINUS expressao      { $$ = criar_no_op_binaria(MINUS, $1, $3); }
    | expressao TIMES expressao      { $$ = criar_no_op_binaria(TIMES, $1, $3); }
    | expressao DIVIDE expressao     { $$ = criar_no_op_binaria(DIVIDE, $1, $3); }
    | expressao MOD expressao        { $$ = criar_no_op_binaria(MOD, $1, $3); }
    | NOT expressao                  { $$ = criar_no_op_unaria(NOT, $2); }
    | MINUS expressao %prec UMINUS   { $$ = criar_no_op_unaria(MINUS, $2); }
    | LPAREN expressao RPAREN        { $$ = $2; }
    | ID                             { $$ = criar_no_id($1); }
    | NUM                            { $$ = criar_no_literal_int($1); }
    | NUM_FLOAT                      { $$ = criar_no_literal_float($1); }
    | CHAR_LITERAL                   { $$ = criar_no_literal_char($1); }
    | STRING_LITERAL                 { $$ = criar_no_literal_string($1); }
    ;

%%

void yyerror(const char *s)
{
    fprintf(stderr, "Erro sintatico: %s\n", s);
}

int main(void)
{
    if (yyparse() == 0)
    {
        printf("Analise sintatica concluida com sucesso!\n");
    }

    return 0;
}
