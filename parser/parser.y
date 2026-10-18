%{
#include <stdio.h>
#include <stdlib.h>
#include "ast.h"

int yylex(void);
void yyerror(const char *s);

NoAST *raiz = NULL;
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

%token IF ELSE WHILE FOR

%token INT FLOAT CHAR

%token RETURN

%type <node> expressao
%type <node> programa lista_comandos comando
%type <node> declaracao lista_declaracoes declaracao_variavel
%type <ival> tipo

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
      lista_comandos                { raiz = criar_no_bloco($1); $$ = raiz; }
    ;

lista_comandos:
      lista_comandos comando        { $$ = ligar_nos($1, $2); }
    | comando                       { $$ = $1; }
    ;

comando:
      IF LPAREN expressao RPAREN comando %prec LOWER_THAN_ELSE
                                     { $$ = criar_no_if($3, $5, NULL); }
    | IF LPAREN expressao RPAREN comando ELSE comando
                                     { $$ = criar_no_if($3, $5, $7); }
    | WHILE LPAREN expressao RPAREN comando
                                     { $$ = criar_no_while($3, $5); }
    | FOR LPAREN expressao SEMI expressao SEMI expressao RPAREN comando
                                     { $$ = criar_no_for($3, $5, $7, $9); }
    | LBRACE lista_comandos RBRACE
                                     { $$ = criar_no_bloco($2); }
    | declaracao                    { $$ = $1; }
    | RETURN expressao SEMI         { $$ = criar_no_return($2); }
    | expressao SEMI                { $$ = $1; }
    ;

declaracao:
      tipo ID SEMI
          { $$ = criar_no_declaracao($1, criar_no_id($2), NULL); }
    | tipo ID ASSIGN expressao SEMI
          { $$ = criar_no_declaracao($1, criar_no_id($2), $4); }
    | tipo lista_declaracoes SEMI
          {
              NoAST *atual = $2;
              while (atual != NULL) {
                  atual->dado.tipo_dado = $1;
                  atual = atual->proximo;
              }
              $$ = $2;
          }
    ;

lista_declaracoes:
      declaracao_variavel                           { $$ = $1; }
    | lista_declaracoes COMMA declaracao_variavel   { $$ = ligar_nos($1, $3); }
    ;

declaracao_variavel:
      ID                      { $$ = criar_no_declaracao(0, criar_no_id($1), NULL); }
    | ID ASSIGN expressao     { $$ = criar_no_declaracao(0, criar_no_id($1), $3); }
    ;

tipo:
      INT    { $$ = INT; }
    | FLOAT  { $$ = FLOAT; }
    | CHAR   { $$ = CHAR; }
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
