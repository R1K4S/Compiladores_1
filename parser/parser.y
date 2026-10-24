%code requires {
#include "ast.h"
}

%{
#include <stdio.h>
#include <stdlib.h>
#include "ast.h"
#include "tabela.h"

extern int yylineno; /* Variável global gerenciada pelo Flex */
int yylex(void);
void yyerror(const char *s);
NoAST* raiz_ast = NULL; // Raiz da AST do programa
int tipo_atual = -1; // Guarda o tipo (INT, FLOAT) durante uma declaração múltipla
%}

%union {
    int int_val;
    float float_val;
    char* str_val;
    NoAST* no;
}

%token <str_val> ID

/* Habilita mensagens detalhadas: "syntax error, unexpected ID, expecting SEMI" */
%define parse.error verbose

%token <int_val> NUM CHAR_LITERAL
%token <float_val> NUM_FLOAT
%token <str_val> STRING_LITERAL

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

%type <no> programa lista_comandos comando declaracao lista_declaracoes declaracao_variavel expressao
%type <int_val> tipo

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
      lista_comandos        { raiz_ast = $1; }
    ;

lista_comandos:
      lista_comandos comando    { $$ = ligar_nos($1, $2); }
    | comando                   { $$ = $1; }
    ;

bloco_inicio: LBRACE { entrar_escopo(); } ;
bloco_fim: RBRACE { sair_escopo(); } ;

comando:
      IF LPAREN expressao RPAREN comando %prec LOWER_THAN_ELSE
                                { $$ = criar_no_if($3, $5, NULL); }
    | IF LPAREN expressao RPAREN comando ELSE comando
                                { $$ = criar_no_if($3, $5, $7); }
    | WHILE LPAREN expressao RPAREN comando
                                { $$ = criar_no_while($3, $5); }
    | bloco_inicio lista_comandos bloco_fim
                                { $$ = criar_no_bloco($2); }
    /*| LBRACE lista_comandos RBRACE*/
    | declaracao                { $$ = $1; }
    | RETURN expressao SEMI     { $$ = criar_no_return($2); }
    | expressao SEMI            { $$ = $1; }
    ;

declaracao:
      tipo lista_declaracoes SEMI     { $$ = $2; }
    ;

lista_declaracoes:
      declaracao_variavel
        { $$ = $1; }
    | lista_declaracoes COMMA declaracao_variavel
        { $$ = ligar_nos($1, $3); }
    ;

tipo:
      INT   { tipo_atual = INT; $$ = INT; }
    | FLOAT { tipo_atual = FLOAT; $$ = FLOAT; }
    | CHAR  { tipo_atual = CHAR; $$ = CHAR; }
    ;

declaracao_variavel:
      ID {
          if (!inserir_variavel($1, tipo_atual)) {
              fprintf(stderr, "Erro Semantico na linha %d: Variavel '%s' ja declarada neste escopo.\n", yylineno, $1);
          }
          $$ = criar_no_declaracao(tipo_atual, criar_no_id($1), NULL);
      }
    | ID ASSIGN expressao {
          if (!inserir_variavel($1, tipo_atual)) {
              fprintf(stderr, "Erro Semantico na linha %d: Variavel '%s' ja declarada neste escopo.\n", yylineno, $1);
          }
          $$ = criar_no_declaracao(tipo_atual, criar_no_id($1), $3);
      }
    ;

expressao:
      expressao ASSIGN expressao  { $$ = criar_no_atribuicao($1, $3); }
    | expressao OR expressao  { $$ = criar_no_op_binaria(OR, $1, $3); }
    | expressao AND expressao  { $$ = criar_no_op_binaria(AND, $1, $3); }
    | expressao EQ expressao  { $$ = criar_no_op_binaria(EQ, $1, $3); }
    | expressao NEQ expressao  { $$ = criar_no_op_binaria(NEQ, $1, $3); }
    | expressao LT expressao  { $$ = criar_no_op_binaria(LT, $1, $3); }
    | expressao GT expressao  { $$ = criar_no_op_binaria(GT, $1, $3); }
    | expressao LE expressao  { $$ = criar_no_op_binaria(LE, $1, $3); }
    | expressao GE expressao  { $$ = criar_no_op_binaria(GE, $1, $3); }
    | expressao PLUS expressao  { $$ = criar_no_op_binaria(PLUS, $1, $3); }
    | expressao MINUS expressao  { $$ = criar_no_op_binaria(MINUS, $1, $3); }
    | expressao TIMES expressao  { $$ = criar_no_op_binaria(TIMES, $1, $3); }
    | expressao DIVIDE expressao  { $$ = criar_no_op_binaria(DIVIDE, $1, $3); }
    | expressao MOD expressao  { $$ = criar_no_op_binaria(MOD, $1, $3); }
    | NOT expressao               { $$ = criar_no_op_unaria(NOT, $2); }
    | MINUS expressao %prec UMINUS { $$ = criar_no_op_unaria(MINUS, $2); }
    | LPAREN expressao RPAREN     { $$ = $2; }
    | ID                          { $$ = criar_no_id($1); }
    | NUM                         { $$ = criar_no_literal_int($1); }
    | NUM_FLOAT                   { $$ = criar_no_literal_float($1); }
    | CHAR_LITERAL                { $$ = criar_no_literal_char((char)$1); }
    | STRING_LITERAL              { $$ = criar_no_literal_string($1); }
    ;

%%
    
void yyerror(const char *s)
{
    fprintf(stderr, "Erro sintático na linha %d: %s\n", yylineno, s);
}

int main(void)
{
    inicializar_tabela();

    if (yyparse() == 0)
    {
        printf("Analise sintatica concluida com sucesso!\n");
    }

    return 0;
}