#ifndef TABELA_H
#define TABELA_H

/* Estrutura de uma variável na tabela */
typedef struct Simbolo {
    char* nome;
    int tipo_dado;          /* pode ser INT, FLOAT, CHAR (tokens do Bison) */
    struct Simbolo* proximo;
} Simbolo;

/* Estrutura do Escopo (Pilha) */
typedef struct Escopo {
    Simbolo* tabela;        /* Cabeça da lista encadeada deste escopo */
    struct Escopo* pai;     /* Ponteiro para o escopo externo */
} Escopo;

void inicializar_tabela();
void entrar_escopo();
void sair_escopo();
int inserir_variavel(const char* nome, int tipo_dado);
Simbolo* buscar_variavel(const char* nome);

#endif