%{
#include <stdio.h>
#include <stdlib.h>
#include "tabela.h"

extern int yylineno; /* Variável global gerenciada pelo Flex */
int yylex(void);
void yyerror(const char *s);
int tipo_atual = -1; // Guarda o tipo (INT, FLOAT) durante uma declaração múltipla
%}

%union {
    int int_val;
    char* str_val;
}

%token <str_val> ID

/* Habilita mensagens detalhadas: "syntax error, unexpected ID, expecting SEMI" */
%define parse.error verbose

%token NUM NUM_FLOAT CHAR_LITERAL STRING_LITERAL

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

bloco_inicio: LBRACE { entrar_escopo(); } ;
bloco_fim: RBRACE { sair_escopo(); } ;

comando:
      IF LPAREN expressao RPAREN comando %prec LOWER_THAN_ELSE
    | IF LPAREN expressao RPAREN comando ELSE comando
    | WHILE LPAREN expressao RPAREN comando
    | bloco_inicio lista_comandos bloco_fim 
    /*| LBRACE lista_comandos RBRACE*/
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
      }
    | ID ASSIGN expressao {
          if (!inserir_variavel($1, tipo_atual)) {
              fprintf(stderr, "Erro Semantico na linha %d: Variavel '%s' ja declarada neste escopo.\n", yylineno, $1);
          }
      }
    ;

expressao:
      expressao ASSIGN expressao
    | expressao OR expressao
    | expressao AND expressao
    | expressao EQ expressao
    | expressao NEQ expressao
    | expressao LT expressao
    | expressao GT expressao
    | expressao LE expressao
    | expressao GE expressao
    | expressao PLUS expressao
    | expressao MINUS expressao
    | expressao TIMES expressao
    | expressao DIVIDE expressao
    | expressao MOD expressao
    | NOT expressao
    | MINUS expressao %prec UMINUS
    | LPAREN expressao RPAREN
    | ID
    | NUM
    | NUM_FLOAT
    | CHAR_LITERAL
    | STRING_LITERAL
    ;

%%
    
void yyerror(const char *s)
{
    fprintf(stderr, "Erro sintático na linha %d: %s\n", yylineno, s);
}

int main(void)
{
    if (yyparse() == 0)
    {
        printf("Analise sintatica concluida com sucesso!\n");
    }

    return 0;
}