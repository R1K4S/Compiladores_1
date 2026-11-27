%code requires {
#include "ast.h"
}

%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "tabela.h"
#include "interpretador.h"

extern int yylineno; /* Variável global gerenciada pelo Flex */
int yylex(void);
void yyerror(const char *s);
NoAST* raiz_ast = NULL; // Raiz da AST do programa
int tipo_atual = -1; // Guarda o tipo (INT, FLOAT) durante uma declaração múltipla
int tipo_retorno_atual = -1; // Tipo de retorno da funcao sendo parseada no momento (-1 = fora de funcao)

/* Total de erros lexicos, sintaticos e semanticos. Se for > 0 o programa NAO e executado
   (Especificacao, Secao 11: programas invalidos sao rejeitados antes da execucao).
   O lexer (lexel.l) tambem incrementa este contador. */
int total_erros = 0;

/* Erros sintaticos ja vistos, e quantos havia ao comecar a funcao atual: se a funcao teve
   erro sintatico, seu corpo esta incompleto e a checagem de return ausente e pulada */
static int erros_sintaticos = 0;
static int erros_sintaticos_inicio_funcao = 0;

/* Reporta um erro semantico em stderr e o contabiliza */
#define ERRO_SEMANTICO(...) do { total_erros++; fprintf(stderr, __VA_ARGS__); } while (0)

/* Definidas no fim do arquivo (lá os tokens, como INT, já existem) */
static void iniciar_funcao(int tipo_retorno, const char* nome, NoAST* params, int linha);
static void verificar_main(void);
static void verificar_chamadas_pendentes(void);
static int lista_sempre_retorna(const NoAST* lista);
static NoAST* criar_binaria_verificada(int op, NoAST* esq, NoAST* dir, int linha);
static NoAST* criar_unaria_verificada(int op, NoAST* operando, int linha);
static NoAST* validar_chamada(const char* nome, NoAST* argumentos, int linha);
static NoAST* validar_scanf(const char* formato, const char* nome_var, int linha);
static NoAST* criar_incremento(const char* nome, int op, int linha);
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

/* Posicao (linha) de cada simbolo: @n.first_line nas acoes, para erros apontarem a linha exata */
%locations

%token <int_val> NUM CHAR_LITERAL
%token <float_val> NUM_FLOAT
%token <str_val> STRING_LITERAL

%token PLUS MINUS TIMES DIVIDE MOD
%token INC DEC

%token EQ NEQ LT GT LE GE

%token AND OR NOT

%token ASSIGN

%token LPAREN RPAREN
%token LBRACE RBRACE
%token SEMI COMMA

%token IF ELSE WHILE

%token FOR PRINTF SCANF

%token INT FLOAT CHAR

%token RETURN

%type <no> programa lista_comandos comando declaracao lista_declaracoes declaracao_variavel expressao
%type <no> for_init
%type <no> corpo_funcao
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
    /* Recuperacao de erro no nivel do programa (ex.: cabecalho de funcao invalido):
       descarta tokens ate o proximo '}' e segue para o proximo item */
    | error RBRACE          { yyerrok; $$ = NULL; }
    ;

definicao_funcao:
      tipo ID LPAREN parametros_opt RPAREN { iniciar_funcao($1, $2, $4, @2.first_line); }
      corpo_funcao
        {
          if (erros_sintaticos == erros_sintaticos_inicio_funcao && !lista_sempre_retorna($7)) {
              ERRO_SEMANTICO("Erro Semantico na linha %d: funcao '%s' pode terminar sem 'return'.\n",
                             @2.first_line, $2);
          }
          restaurar_escopo(NIVEL_GLOBAL); tipo_retorno_atual = -1;
          $$ = criar_no_funcao($1, $2, $4, criar_no_bloco($7));
        }
    ;

/* Corpo da funcao. As duas ultimas alternativas recuperam erros sintaticos:
   descartam tokens ate o '}' e continuam a analise do resto do programa. */
corpo_funcao:
      LBRACE lista_comandos RBRACE          { $$ = $2; }
    | LBRACE RBRACE                         { $$ = NULL; }   /* corpo vazio */
    | LBRACE lista_comandos error RBRACE    { yyerrok; $$ = $2; }
    | LBRACE error RBRACE                   { yyerrok; $$ = NULL; }
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

/* O for tem escopo proprio: a variavel de "for (int i = 0; ...)" so existe dentro do laco */
for_inicio: FOR { entrar_escopo(); } ;
for_init:
      declaracao                { $$ = $1; }
    | expressao SEMI            { $$ = $1; }
    ;

comando:
      IF LPAREN expressao RPAREN comando %prec LOWER_THAN_ELSE
                                { $$ = criar_no_if($3, $5, NULL); }
    | IF LPAREN expressao RPAREN comando ELSE comando
                                { $$ = criar_no_if($3, $5, $7); }
    | WHILE LPAREN expressao RPAREN comando
                                { $$ = criar_no_while($3, $5); }
    | for_inicio LPAREN for_init expressao SEMI expressao RPAREN comando
                                { sair_escopo(); $$ = criar_no_for($3, $4, $6, $8); }
    | bloco_inicio lista_comandos bloco_fim
                                { $$ = criar_no_bloco($2); }
    | bloco_inicio bloco_fim    { $$ = criar_no_bloco(NULL); }   /* bloco vazio: { } */
    /* Recuperacao de erros sintaticos (semana 05): o Bison descarta tokens ate achar
       ';' ou '}' e a analise continua, para reportar varios erros numa execucao.
       Como ha erro, o programa nao sera executado (ver main). */
    | bloco_inicio lista_comandos error bloco_fim
                                { yyerrok; $$ = criar_no_bloco($2); }
    | bloco_inicio error bloco_fim
                                { yyerrok; $$ = NULL; }
    | error SEMI                { yyerrok; $$ = NULL; }
    | declaracao                { $$ = $1; }
    | SCANF LPAREN STRING_LITERAL COMMA ID RPAREN SEMI { $$ = validar_scanf($3, $5, @1.first_line); }
    | PRINTF LPAREN STRING_LITERAL RPAREN SEMI { $$ = criar_no_printf($3, NULL); }
    | PRINTF LPAREN STRING_LITERAL COMMA lista_argumentos RPAREN SEMI { $$ = criar_no_printf($3, $5); }
    | RETURN expressao SEMI {
          int t = tipo_da_expressao($2);
          if (tipo_retorno_atual == -1) {
              ERRO_SEMANTICO("Erro Semantico na linha %d: 'return' fora de uma funcao.\n", @1.first_line);
          } else if (t != -1 && !tipos_compativeis(tipo_retorno_atual, t)){
              ERRO_SEMANTICO(
                  "Erro Semantico na linha %d: tipo de retorno incompativel (esperado '%s').\n",
                  @1.first_line, nome_token(tipo_retorno_atual));
          }
          $$ = criar_no_return($2);
      }
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
              ERRO_SEMANTICO("Erro Semantico na linha %d: Variavel '%s' ja declarada neste escopo.\n", @1.first_line, $1);
          }
          $$ = criar_no_declaracao(tipo_atual, criar_no_id($1), NULL);
      }
    | ID ASSIGN expressao {
          if (!inserir_variavel($1, tipo_atual)) {
              ERRO_SEMANTICO("Erro Semantico na linha %d: Variavel '%s' ja declarada neste escopo.\n", @1.first_line, $1);
          }
          int td = tipo_da_expressao($3);
          if (td != -1 && !tipos_compativeis(tipo_atual, td)) {
              ERRO_SEMANTICO(
                  "Erro Semantico na linha %d: inicializacao incompativel para '%s' (esperado '%s').\n",
                  @1.first_line, $1, nome_token(tipo_atual));
          }
          $$ = criar_no_declaracao(tipo_atual, criar_no_id($1), $3);
      }
    ;

expressao:
      expressao ASSIGN expressao {
          int te = tipo_da_expressao($1);
          int td = tipo_da_expressao($3);
          if (te != -1 && td != -1 && !tipos_compativeis(te, td)) {
              ERRO_SEMANTICO(
                  "Erro Semantico na linha %d: atribuicao incompativel (tipo '%s' nao aceita o valor atribuido).\n",
                  @2.first_line, nome_token(te));
          }
          $$ = criar_no_atribuicao($1, $3);
      }
    | expressao OR expressao      { $$ = criar_binaria_verificada(OR, $1, $3, @2.first_line); }
    | expressao AND expressao     { $$ = criar_binaria_verificada(AND, $1, $3, @2.first_line); }
    | expressao EQ expressao      { $$ = criar_binaria_verificada(EQ, $1, $3, @2.first_line); }
    | expressao NEQ expressao     { $$ = criar_binaria_verificada(NEQ, $1, $3, @2.first_line); }
    | expressao LT expressao      { $$ = criar_binaria_verificada(LT, $1, $3, @2.first_line); }
    | expressao GT expressao      { $$ = criar_binaria_verificada(GT, $1, $3, @2.first_line); }
    | expressao LE expressao      { $$ = criar_binaria_verificada(LE, $1, $3, @2.first_line); }
    | expressao GE expressao      { $$ = criar_binaria_verificada(GE, $1, $3, @2.first_line); }
    | expressao PLUS expressao    { $$ = criar_binaria_verificada(PLUS, $1, $3, @2.first_line); }
    | expressao MINUS expressao   { $$ = criar_binaria_verificada(MINUS, $1, $3, @2.first_line); }
    | expressao TIMES expressao   { $$ = criar_binaria_verificada(TIMES, $1, $3, @2.first_line); }
    | expressao DIVIDE expressao  { $$ = criar_binaria_verificada(DIVIDE, $1, $3, @2.first_line); }
    | expressao MOD expressao     { $$ = criar_binaria_verificada(MOD, $1, $3, @2.first_line); }
    | NOT expressao                { $$ = criar_unaria_verificada(NOT, $2, @1.first_line); }
    | MINUS expressao %prec UMINUS { $$ = criar_unaria_verificada(MINUS, $2, @1.first_line); }
    | LPAREN expressao RPAREN     { $$ = $2; }
    | ID {
          if (!buscar_variavel($1)) {
              ERRO_SEMANTICO("Erro Semantico na linha %d: Variavel '%s' nao declarada.\n", @1.first_line, $1);
          }
          $$ = criar_no_id($1);
      }
    | ID LPAREN argumentos_opt RPAREN { $$ = validar_chamada($1, $3, @1.first_line); }
    | ID INC                      { $$ = criar_incremento($1, PLUS, @1.first_line); }
    | ID DEC                      { $$ = criar_incremento($1, MINUS, @1.first_line); }
    | INC ID                      { $$ = criar_incremento($2, PLUS, @2.first_line); }
    | DEC ID                      { $$ = criar_incremento($2, MINUS, @2.first_line); }
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
static void iniciar_funcao(int tipo_retorno, const char* nome, NoAST* params, int linha) {
    /* Funcoes so existem no nivel do programa: se um erro sintatico anterior deixou
       escopos abertos, volta ao global antes de registrar esta funcao */
    restaurar_escopo(NIVEL_GLOBAL);
    erros_sintaticos_inicio_funcao = erros_sintaticos;
    tipo_retorno_atual = tipo_retorno;

    int n = 0;
    for (NoAST* p = params; p != NULL; p = p->proximo) n++;

    int* tipos = NULL;
    if (n > 0) {
        tipos = (int*)malloc(n * sizeof(int));
        int i = 0;
        for (NoAST* p = params; p != NULL; p = p->proximo) tipos[i++] = p->dado.tipo_dado;
    }

    if (strcmp(nome, "main") == 0 && (tipo_retorno != INT || n != 0)) {
        ERRO_SEMANTICO("Erro Semantico na linha %d: 'main' deve ter a assinatura 'int main()'.\n", linha);
    }

    if (!inserir_funcao(nome, tipo_retorno, n, tipos)) {
        ERRO_SEMANTICO("Erro Semantico na linha %d: '%s' ja declarado neste escopo.\n", linha, nome);
    }
    free(tipos);

    entrar_escopo();
    for (NoAST* p = params; p != NULL; p = p->proximo) {
        const char* nome_param = p->esq->dado.string_val;
        if (!inserir_variavel(nome_param, p->dado.tipo_dado)) {
            ERRO_SEMANTICO("Erro Semantico na linha %d: Parametro '%s' duplicado na funcao '%s'.\n", linha, nome_param, nome);
        }
    }
}

/* Constroi um no de operacao binaria, verificando antes que nenhum operando seja string */
static NoAST* criar_binaria_verificada(int op, NoAST* esq, NoAST* dir, int linha) {
    int te = tipo_da_expressao(esq);
    int td = tipo_da_expressao(dir);
    if (te == TIPO_STRING || td == TIPO_STRING) {
        ERRO_SEMANTICO("Erro Semantico na linha %d: operador '%s' nao pode ser aplicado a uma string.\n",
                linha, nome_token(op));
    }
    return criar_no_op_binaria(op, esq, dir);
}

/* Constroi um no de operacao unaria, verificando antes que o operando nao seja string */
static NoAST* criar_unaria_verificada(int op, NoAST* operando, int linha) {
    if (tipo_da_expressao(operando) == TIPO_STRING) {
        ERRO_SEMANTICO("Erro Semantico na linha %d: operador '%s' nao pode ser aplicado a uma string.\n",
                linha, nome_token(op));
    }
    return criar_no_op_unaria(op, operando);
}

/* Verifica existencia da funcao, aridade e tipos dos argumentos antes de construir o no de chamada */
/* Confere aridade e tipos de uma chamada contra a assinatura da funcao 's' */
static void checar_assinatura(const Simbolo* s, const char* nome, int n_args, const int* tipos_args, int linha) {
    if (n_args != s->num_params) {
        ERRO_SEMANTICO(
            "Erro Semantico na linha %d: funcao '%s' espera %d argumento(s), recebeu %d.\n",
            linha, nome, s->num_params, n_args);
        return;
    }
    for (int i = 0; i < n_args; i++) {
        int t_arg = tipos_args[i];
        int t_param = s->tipos_params[i];
        if (t_arg != -1 && !tipos_compativeis(t_param, t_arg)) {
            ERRO_SEMANTICO(
                "Erro Semantico na linha %d: argumento %d de '%s' incompativel (esperado '%s').\n",
                linha, i + 1, nome, nome_token(t_param));
        }
    }
}

/* Chamadas a funcoes ainda nao vistas (definidas mais abaixo no arquivo, ou recursao
   mutua). Sao conferidas no fim do parse, quando todas as funcoes ja foram registradas. */
typedef struct ChamadaPendente {
    char* nome;
    int n_args;
    int* tipos_args;
    int linha;
    struct ChamadaPendente* proximo;
} ChamadaPendente;

static ChamadaPendente* pendentes = NULL;
static ChamadaPendente* ultima_pendente = NULL;

/* Verifica existencia da funcao, aridade e tipos dos argumentos antes de construir o no de chamada */
static NoAST* validar_chamada(const char* nome, NoAST* argumentos, int linha) {
    int n_args = 0;
    for (NoAST* a = argumentos; a != NULL; a = a->proximo) n_args++;

    /* Os tipos dos argumentos sao calculados agora, no escopo da chamada */
    int* tipos = (int*)malloc((n_args > 0 ? n_args : 1) * sizeof(int));
    int i = 0;
    for (NoAST* a = argumentos; a != NULL; a = a->proximo) tipos[i++] = tipo_da_expressao(a);

    Simbolo* s = buscar_variavel(nome);
    if (s == NULL) {
        /* Ainda nao declarada: pode ser definida mais abaixo. Confere no fim (verificar_chamadas_pendentes) */
        ChamadaPendente* p = (ChamadaPendente*)malloc(sizeof(ChamadaPendente));
        p->nome = strdup(nome);
        p->n_args = n_args;
        p->tipos_args = tipos;
        p->linha = linha;
        p->proximo = NULL;
        if (ultima_pendente) ultima_pendente->proximo = p; else pendentes = p;
        ultima_pendente = p;
        return criar_no_chamada(nome, argumentos);
    }

    if (!s->eh_funcao) {
        ERRO_SEMANTICO("Erro Semantico na linha %d: '%s' nao e uma funcao.\n", linha, nome);
    } else {
        checar_assinatura(s, nome, n_args, tipos, linha);
    }
    free(tipos);
    return criar_no_chamada(nome, argumentos);
}

/* Chamada apos o parse, no escopo global: confere as chamadas que ficaram pendentes */
static void verificar_chamadas_pendentes(void) {
    restaurar_escopo(NIVEL_GLOBAL);
    for (ChamadaPendente* p = pendentes; p != NULL; p = p->proximo) {
        Simbolo* s = buscar_variavel(p->nome);
        if (s == NULL) {
            ERRO_SEMANTICO("Erro Semantico na linha %d: funcao '%s' nao declarada.\n", p->linha, p->nome);
        } else if (!s->eh_funcao) {
            ERRO_SEMANTICO("Erro Semantico na linha %d: '%s' nao e uma funcao.\n", p->linha, p->nome);
        } else {
            checar_assinatura(s, p->nome, p->n_args, p->tipos_args, p->linha);
        }
    }
}

/* ---- Checagem de 'return' ausente (Especificacao, Secao 11, item 7) ----
   Um comando "sempre retorna" se for um return, um bloco cuja lista sempre retorna,
   ou um if COM else em que os dois ramos sempre retornam. Lacos nao contam
   (a condicao pode ser falsa ja na primeira vez). */
static int lista_sempre_retorna(const NoAST* lista);

static int comando_sempre_retorna(const NoAST* no) {
    if (no == NULL) return 0;
    switch (no->tipo) {
        case NO_RETURN: return 1;
        case NO_BLOCO:  return lista_sempre_retorna(no->esq);
        case NO_IF:     return no->centro != NULL
                            && comando_sempre_retorna(no->dir)
                            && comando_sempre_retorna(no->centro);
        default:        return 0;
    }
}

static int lista_sempre_retorna(const NoAST* lista) {
    for (; lista != NULL; lista = lista->proximo) {
        if (comando_sempre_retorna(lista)) return 1;
    }
    return 0;
}

/* Mapeia o especificador do scanf para o token de tipo esperado, ou -1 se invalido */
static int tipo_esperado_scanf(const char* formato) {
    if (strcmp(formato, "%d") == 0) return INT;
    if (strcmp(formato, "%f") == 0) return FLOAT;
    if (strcmp(formato, "%c") == 0) return CHAR;
    return -1;
}

/* Verifica formato suportado, existencia da variavel e compatibilidade de tipo (exata, sem promocao) */
static NoAST* validar_scanf(const char* formato, const char* nome_var, int linha) {
    int esperado = tipo_esperado_scanf(formato);
    if (esperado == -1) {
        ERRO_SEMANTICO(
            "Erro Semantico na linha %d: scanf aceita apenas os formatos \"%%d\", \"%%f\" ou \"%%c\".\n",
            linha);
    }

    Simbolo* s = buscar_variavel(nome_var);
    if (s == NULL) {
        ERRO_SEMANTICO("Erro Semantico na linha %d: Variavel '%s' nao declarada.\n", linha, nome_var);
    } else if (esperado != -1 && s->tipo_dado != esperado) {
        ERRO_SEMANTICO(
            "Erro Semantico na linha %d: scanf com formato '%s' espera variavel '%s', mas '%s' e '%s'.\n",
            linha, formato, nome_token(esperado), nome_var, nome_token(s->tipo_dado));
    }

    return criar_no_scanf(formato, criar_no_id(nome_var));
}

/* i++, ++i, i-- e --i sao acucar sintatico para "i = i + 1" (ou "- 1"): nao precisam de
   nada novo no interpretador. O valor da expressao e o valor ja atualizado. */
static NoAST* criar_incremento(const char* nome, int op, int linha) {
    if (!buscar_variavel(nome)) {
        ERRO_SEMANTICO("Erro Semantico na linha %d: Variavel '%s' nao declarada.\n", linha, nome);
    }
    return criar_no_atribuicao(criar_no_id(nome),
        criar_no_op_binaria(op, criar_no_id(nome), criar_no_literal_int(1)));
}

/* Todo programa precisa ter uma função 'main'.
   Chamada após o parse, quando o escopo global é o escopo atual. */
static void verificar_main(void) {
    restaurar_escopo(NIVEL_GLOBAL);
    Simbolo* m = buscar_variavel("main");
    if (m == NULL || !m->eh_funcao) {
        ERRO_SEMANTICO("Erro Semantico: funcao 'main' nao encontrada. Todo programa precisa de 'int main()'.\n");
    }
}

void yyerror(const char *s)
{
    total_erros++;
    erros_sintaticos++;
    fprintf(stderr, "Erro sintático na linha %d: %s\n", yylineno, s);
}

extern FILE* yyin; /* Entrada do Flex: por padrao stdin */

/* Uso: ./build/compilador [-ast] programa.c
          -ast  imprime tambem a AST (antes de executar) e as variaveis globais (ao final)
   Sem arquivo, le o programa do stdin (ai o scanf nao tera entrada). */
int main(int argc, char** argv)
{
    int mostrar_ast = 0;
    const char* arquivo = NULL;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-ast") == 0) {
            mostrar_ast = 1;
        } else if (arquivo == NULL && argv[i][0] != '-') {
            arquivo = argv[i];
        } else {
            fprintf(stderr, "Uso: %s [-ast] [arquivo.c]\n", argv[0]);
            return 1;
        }
    }
    if (arquivo != NULL) {
        yyin = fopen(arquivo, "r");
        if (yyin == NULL) {
            perror(arquivo);
            return 1;
        }
    }

    inicializar_tabela();

    int resultado_parse = yyparse();
    if (resultado_parse == 0) {
        verificar_main();
        verificar_chamadas_pendentes();
    }

    /* Qualquer erro (lexico, sintatico ou semantico) impede a execucao e sai com codigo 1 */
    if (resultado_parse != 0 || total_erros > 0) {
        fprintf(stderr, "%d erro(s) encontrado(s). O programa nao foi executado.\n",
                total_erros > 0 ? total_erros : 1);
        return 1;
    }

    if (mostrar_ast) {
        printf("Analise sintatica concluida com sucesso!\n");
        imprimir_ast(raiz_ast);
    }

    /* O codigo de saida do processo e o valor retornado por 'main' do programa MiniC */
    return executar_programa(raiz_ast, mostrar_ast);
}