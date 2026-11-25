#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tabela.h"

Escopo* escopo_atual = NULL;

void inicializar_tabela() {
    entrar_escopo(); // Cria o escopo global base
}

void entrar_escopo() {
    Escopo* novo_escopo = (Escopo*)malloc(sizeof(Escopo));
    novo_escopo->tabela = NULL;
    novo_escopo->pai = escopo_atual;
    escopo_atual = novo_escopo;
}

void sair_escopo() {
    if (!escopo_atual) return;
    
    Escopo* escopo_removido = escopo_atual;
    escopo_atual = escopo_atual->pai;
    
    // liberando a memória de todas as variáveis do escopo que esta sendo fechado
    Simbolo* atual = escopo_removido->tabela;
    while (atual != NULL) {
        Simbolo* proximo = atual->proximo;
        free(atual->nome);
        free(atual->tipos_params);
        free(atual);
        atual = proximo;
    }
    free(escopo_removido);
}

int inserir_variavel(const char* nome, int tipo_dado) {
    if (!escopo_atual) return 0;

    // verifica se ja existe NO MESMO ESCOPO
    Simbolo* atual = escopo_atual->tabela;
    while (atual != NULL) {
        if (strcmp(atual->nome, nome) == 0) return 0; // Erro: já declarada neste escopo
        atual = atual->proximo;
    }

    // inserindo no início da lista do escopo atual
    Simbolo* novo_simbolo = (Simbolo*)malloc(sizeof(Simbolo));
    novo_simbolo->nome = strdup(nome);
    novo_simbolo->tipo_dado = tipo_dado;
    novo_simbolo->eh_funcao = 0;
    novo_simbolo->num_params = 0;
    novo_simbolo->tipos_params = NULL;
    novo_simbolo->proximo = escopo_atual->tabela;
    escopo_atual->tabela = novo_simbolo;
    
    return 1; // deu certo
}

/* Funções e variáveis dividem o mesmo espaço de nomes, como em C */
int inserir_funcao(const char* nome, int tipo_retorno, int num_params, const int* tipos_params) {
    if (!inserir_variavel(nome, tipo_retorno)) return 0;

    Simbolo* s = escopo_atual->tabela;  // o símbolo recém-inserido fica na cabeça da lista
    s->eh_funcao = 1;
    s->num_params = num_params;
    if (num_params > 0) {
        s->tipos_params = (int*)malloc(num_params * sizeof(int));
        memcpy(s->tipos_params, tipos_params, num_params * sizeof(int));
    }
    return 1;
}

Simbolo* buscar_variavel(const char* nome) {
    Escopo* escopo_busca = escopo_atual;
    
    // usando uma busca de dentro para fora
    while (escopo_busca != NULL) {
        Simbolo* atual = escopo_busca->tabela;
        while (atual != NULL) {
            if (strcmp(atual->nome, nome) == 0) return atual;
            atual = atual->proximo;
        }
        escopo_busca = escopo_busca->pai;
    }
    return NULL; // variavel n declarada
}