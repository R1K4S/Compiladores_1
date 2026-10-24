%code requires {
#include "ast.h"
}

%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "tabela.h"

extern int yylineno; /* Variável global gerenciada pelo Flex */
int yylex(void);
void yyerror(const char *s);
NoAST* raiz_ast = NULL; // Raiz da AST do programa
int tipo_atual = -1; // Guarda o tipo (INT, FLOAT) durante uma declaração múltipla

/* Definidas no fim do arquivo (lá os tokens, como INT, já existem) */
static void iniciar_funcao(int tipo_retorno, const char* nome, NoAST* params);
static void verificar_main(void);
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
%type <no> lista_itens item definicao_funcao parametros_opt lista_parametros parametro argumentos_opt lista_argumentos
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
      lista_itens           { raiz_ast = $1; }
    ;

/* No topo do programa: comandos e definições de função */
lista_itens:
      lista_itens item      { $$ = ligar_nos($1, $2); }
    | item                  { $$ = $1; }
    ;

item:
      comando               { $$ = $1; }
    | definicao_funcao      { $$ = $1; }
    ;

definicao_funcao:
      tipo ID LPAREN parametros_opt RPAREN { iniciar_funcao($1, $2, $4); }
      LBRACE lista_comandos RBRACE
        { sair_escopo(); $$ = criar_no_funcao($1, $2, $4, criar_no_bloco($8)); }
    ;

parametros_opt:
      /* vazio */           { $$ = NULL; }
    | lista_parametros      { $$ = $1; }
    ;

lista_parametros:
      parametro                        { $$ = $1; }
    | lista_parametros COMMA parametro { $$ = ligar_nos($1, $3); }
    ;

parametro:
      tipo ID               { $$ = criar_no_declaracao($1, criar_no_id($2), NULL); }
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
    | ID {
          if (!buscar_variavel($1)) {
              fprintf(stderr, "Erro Semantico na linha %d: Variavel '%s' nao declarada.\n", yylineno, $1);
          }
          $$ = criar_no_id($1);
      }
    | ID LPAREN argumentos_opt RPAREN { $$ = criar_no_chamada($1, $3); }
    | NUM                         { $$ = criar_no_literal_int($1); }
    | NUM_FLOAT                   { $$ = criar_no_literal_float($1); }
    | CHAR_LITERAL                { $$ = criar_no_literal_char((char)$1); }
    | STRING_LITERAL              { $$ = criar_no_literal_string($1); }
    ;

argumentos_opt:
      /* vazio */           { $$ = NULL; }
    | lista_argumentos      { $$ = $1; }
    ;

lista_argumentos:
      expressao                        { $$ = $1; }
    | lista_argumentos COMMA expressao { $$ = ligar_nos($1, $3); }
    ;

%%

/* Chamada logo após o ')' de uma definição de função: registra a função na tabela
   (no escopo atual), abre o escopo da função e insere os parâmetros nele. */
static void iniciar_funcao(int tipo_retorno, const char* nome, NoAST* params) {
    int n = 0;
    for (NoAST* p = params; p != NULL; p = p->proximo) n++;

    int* tipos = NULL;
    if (n > 0) {
        tipos = (int*)malloc(n * sizeof(int));
        int i = 0;
        for (NoAST* p = params; p != NULL; p = p->proximo) tipos[i++] = p->dado.tipo_dado;
    }

    if (strcmp(nome, "main") == 0 && (tipo_retorno != INT || n != 0)) {
        fprintf(stderr, "Erro Semantico na linha %d: 'main' deve ter a assinatura 'int main()'.\n", yylineno);
    }

    if (!inserir_funcao(nome, tipo_retorno, n, tipos)) {
        fprintf(stderr, "Erro Semantico na linha %d: '%s' ja declarado neste escopo.\n", yylineno, nome);
    }
    free(tipos);

    entrar_escopo();
    for (NoAST* p = params; p != NULL; p = p->proximo) {
        const char* nome_param = p->esq->dado.string_val;
        if (!inserir_variavel(nome_param, p->dado.tipo_dado)) {
            fprintf(stderr, "Erro Semantico na linha %d: Parametro '%s' duplicado na funcao '%s'.\n", yylineno, nome_param, nome);
        }
    }
}

/* Todo programa precisa ter uma função 'main'.
   Chamada após o parse, quando o escopo global é o escopo atual. */
static void verificar_main(void) {
    Simbolo* m = buscar_variavel("main");
    if (m == NULL || !m->eh_funcao) {
        fprintf(stderr, "Erro Semantico: funcao 'main' nao encontrada. Todo programa precisa de 'int main()'.\n");
    }
}

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
        imprimir_ast(raiz_ast);
        verificar_main();
    }

    return 0;
}