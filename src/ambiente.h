#ifndef AMBIENTE_H
#define AMBIENTE_H

/* ===================== Valores em tempo de execução ===================== */

typedef enum { VAL_INT, VAL_FLOAT, VAL_CHAR } TipoValor;

typedef struct {
    TipoValor tipo;
    union {
        int i;      /* VAL_INT e VAL_CHAR */
        float f;    /* VAL_FLOAT */
    } dado;
} Valor;

Valor valor_int(int i);
Valor valor_float(float f);
Valor valor_char(char c);
int   valor_como_int(Valor v);          /* float é truncado */
float valor_como_float(Valor v);
int   valor_verdadeiro(Valor v);        /* diferente de zero */
Valor converter_valor(Valor v, TipoValor destino);

/* ===================== Ambiente (pilha de escopos) ===================== */

/* Uma variável em execução. O tipo declarado é o tipo do próprio 'valor':
   toda escrita converte o novo valor para esse tipo. */
typedef struct VarExec {
    char* nome;
    Valor valor;
    struct VarExec* proximo;
} VarExec;

typedef struct EscopoExec {
    VarExec* vars;              /* variáveis deste escopo */
    struct EscopoExec* pai;     /* escopo que estava ativo antes deste */
    int eh_funcao;              /* 1 = escopo-base de uma chamada de função */
} EscopoExec;

void ambiente_iniciar(void);                /* cria o escopo global */
void ambiente_entrar_escopo(void);          /* bloco { ... } */
void ambiente_entrar_funcao(void);          /* chamada de função: só enxerga o global */
void ambiente_sair_escopo(void);            /* descarta o escopo atual e suas variáveis */

/* Cria a variável no escopo atual. Retorna 0 se o nome já existe nele. */
int ambiente_declarar(const char* nome, Valor inicial);

VarExec* ambiente_buscar(const char* nome);

/* Leitura/escrita. Retornam 0 se a variável não existe.
   A escrita converte para o tipo da variável e devolve o valor gravado em 'gravado' (pode ser NULL). */
int ambiente_ler(const char* nome, Valor* saida);
int ambiente_escrever(const char* nome, Valor novo, Valor* gravado);

void ambiente_imprimir_global(void);        /* lista as variáveis globais (depuração) */

#endif