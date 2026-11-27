#ifndef TABELA_H
#define TABELA_H

/* Estrutura de uma variável na tabela */
typedef struct Simbolo {
    char* nome;
    int tipo_dado;          /* variável: seu tipo | função: tipo de retorno (INT, FLOAT, CHAR) */
    int eh_funcao;          /* 1 se for função, 0 se for variável */
    int num_params;         /* só para função: quantidade de parâmetros */
    int* tipos_params;      /* só para função: tipo de cada parâmetro (NULL se não houver) */
    struct Simbolo* proximo;
} Simbolo;

/* Estrutura do Escopo (Pilha) */
typedef struct Escopo {
    Simbolo* tabela;        /* Cabeça da lista encadeada deste escopo */
    struct Escopo* pai;     /* Ponteiro para o escopo externo */
} Escopo;

/* Nivel do escopo global (criado por inicializar_tabela) */
#define NIVEL_GLOBAL 1

void inicializar_tabela();
void entrar_escopo();
void sair_escopo();

/* Quantos escopos estao abertos (1 = so o global) */
int nivel_escopo(void);

/* Fecha escopos ate restar 'nivel' abertos. Usado para ressincronizar a tabela
   depois de um erro sintatico, quando o Bison descarta um '}' que fecharia um escopo. */
void restaurar_escopo(int nivel);
int inserir_variavel(const char* nome, int tipo_dado);
int inserir_funcao(const char* nome, int tipo_retorno, int num_params, const int* tipos_params);
Simbolo* buscar_variavel(const char* nome);

#endif