%{
#include <stdio.h>
#include <stdlib.h>

int yylex(void);
void yyerror(const char *s);
%}

%token NUM
%token PLUS MINUS TIMES DIVIDE MOD
%token EQ NEQ LT GT LE GE
%token AND OR NOT
%token ASSIGN
%token LPAREN RPAREN LBRACE RBRACE SEMI COMMA
%token IF ELSE WHILE

/* Solução para a ambiguidade do if-else (Dangling-Else) */
%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

/* Precedência dos operadores de expressão (Especificação, Seção 5.6),
   declarada da mais baixa para a mais alta -- ordem exigida pelo Bison */
%right ASSIGN
%left OR
%left AND
%left EQ NEQ
%left LT GT LE GE
%left PLUS MINUS
%left TIMES DIVIDE MOD
%right NOT UMINUS

%%

/* A primeira regra define o ponto de partida do Parser */
programa:
    lista_comandos
    ;

/* Permite que o programa tenha um ou múltiplos comandos em sequência */
lista_comandos:
    lista_comandos comando
  | comando
  ;

comando:
      /* Regra do IF simples (tem precedência menor que o ELSE) */
      IF LPAREN expressao RPAREN comando %prec LOWER_THAN_ELSE

      /* Regra do IF-ELSE */
    | IF LPAREN expressao RPAREN comando ELSE comando

      /* Regra do WHILE */
    | WHILE LPAREN expressao RPAREN comando

      /* Regra para blocos de código com chaves { } */
    | LBRACE lista_comandos RBRACE

      /* Regra base: permite que um comando seja apenas uma expressão */
    | expressao SEMI
    ;

expressao:
    expressao OR expressao
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
  | NUM
  ;

%%

void yyerror(const char *s) {
    fprintf(stderr, "Erro sintático: %s\n", s);
}

int main(void) {
    /* yydebug = 1; */ /* Descomente para debugar os passos do Bison */
    if (yyparse() == 0) {
        printf("Analise sintatica concluida com sucesso!\n");
    }
    return 0;
}