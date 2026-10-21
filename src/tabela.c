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
    novo_simbolo->proximo = escopo_atual->tabela;
    escopo_atual->tabela = novo_simbolo;
    
    return 1; // deu certo
}

Simbolo* buscar_variavel(const char* nome) {
    Escopo* escopo_busca = escopo_atual;
    
    // usando uma busca de dentro para fora
     (escopo_busca != NULL) {
        Simbolo* atual = escopo_busca->tabela;
        while (atual != NULL) {
            if (strcmp(atual->nome, nome) == 0) return atual;
            atual = atual->proximo;
        }
        escopo_busca = escopo_busca->pai;
    }
    return NULL; // variavel n declarada
}