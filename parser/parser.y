%{
#include <stdio.h>
#include <stdlib.h>

int yylex(void);
void yyerror(const char *s);
%}

%token NUM PLUS MINUS TIMES DIVIDE LPAREN RPAREN
%token IF ELSE WHILE LBRACE RBRACE

/* Solução para a ambiguidade do if-else (Dangling-Else) */
%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

/* Precedência matemática para evitar conflitos de Shift/Reduce */
%left PLUS MINUS
%left TIMES DIVIDE

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
    | expressao
    ;
    
expressao:
    expressao PLUS expressao
  | expressao MINUS expressao
  | expressao TIMES expressao
  | expressao DIVIDE expressao
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