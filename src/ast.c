#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "ast.h"

/* Função auxiliar privada para alocar e inicializar um nó vazio */
static NoAST* alocar_no(TipoNo tipo) {
    NoAST* no = (NoAST*)malloc(sizeof(NoAST));
    if (!no) {
        fprintf(stderr, "Erro fatal: Falha na alocacao de memoria para a AST.\n");
        exit(1);
    }
    no->tipo = tipo;
    no->esq = NULL;
    no->dir = NULL;
    no->centro = NULL;
    no->proximo = NULL;
    return no;
}

NoAST* criar_no_literal_int(int valor) {
    NoAST* no = alocar_no(NO_LITERAL_INT);
    no->dado.valor_int = valor;
    return no;
}

NoAST* criar_no_literal_float(float valor) {
    NoAST* no = alocar_no(NO_LITERAL_FLOAT);
    no->dado.valor_float = valor;
    return no;
}

NoAST* criar_no_literal_char(char valor) {
    NoAST* no = alocar_no(NO_LITERAL_CHAR);
    // Armazenando char dentro do inteiro da union para simplificar
    no->dado.valor_int = (int)valor;
    return no;
}

NoAST* criar_no_literal_string(const char* valor) {
    NoAST* no = alocar_no(NO_LITERAL_STRING);
    no->dado.string_val = strdup(valor);
    return no;
}

NoAST* criar_no_id(const char* nome) {
    NoAST* no = alocar_no(NO_ID);
    no->dado.string_val = strdup(nome);
    return no;
}

NoAST* criar_no_op_binaria(int op, NoAST* esq, NoAST* dir) {
    NoAST* no = alocar_no(NO_OP_BINARIA);
    no->dado.op = op;
    no->esq = esq;
    no->dir = dir;
    return no;
}

NoAST* criar_no_op_unaria(int op, NoAST* esq) {
    NoAST* no = alocar_no(NO_OP_UNARIA);
    no->dado.op = op;
    no->esq = esq;
    return no;
}

NoAST* criar_no_atribuicao(NoAST* id, NoAST* expressao) {
    NoAST* no = alocar_no(NO_ATRIBUICAO);
    no->esq = id;
    no->dir = expressao;
    return no;
}

NoAST* criar_no_declaracao(int tipo_dado, NoAST* id, NoAST* expressao) {
    NoAST* no = alocar_no(NO_DECLARACAO);
    no->dado.tipo_dado = tipo_dado;
    no->esq = id;
    no->dir = expressao; // Pode ser NULL se for apenas "int x;"
    return no;
}

NoAST* criar_no_if(NoAST* condicao, NoAST* bloco_if, NoAST* bloco_else) {
    NoAST* no = alocar_no(NO_IF);
    no->esq = condicao;
    no->dir = bloco_if;
    no->centro = bloco_else; // Pode ser NULL se não houver 'else'
    return no;
}

NoAST* criar_no_while(NoAST* condicao, NoAST* bloco) {
    NoAST* no = alocar_no(NO_WHILE);
    no->esq = condicao;
    no->dir = bloco;
    return no;
}

NoAST* criar_no_for(NoAST* init, NoAST* condicao, NoAST* incremento, NoAST* corpo) {
    NoAST* no = alocar_no(NO_FOR);
    init->proximo = condicao;
    condicao->proximo = incremento;
    no->esq = init;
    no->dir = corpo;
    return no;
}

NoAST* criar_no_bloco(NoAST* lista_comandos) {
    NoAST* no = alocar_no(NO_BLOCO);
    no->esq = lista_comandos;
    return no;
}

NoAST* criar_no_return(NoAST* expressao) {
    NoAST* no = alocar_no(NO_RETURN);
    no->esq = expressao;
    return no;
}

NoAST* criar_no_funcao(int tipo_retorno, const char* nome, NoAST* parametros, NoAST* bloco) {
    NoAST* no = alocar_no(NO_FUNCAO);
    no->dado.tipo_dado = tipo_retorno;
    no->dado.string_val = strdup(nome);
    no->esq = parametros;
    no->dir = bloco;
    return no;
}

/* Conecta comandos sequenciais (ex: dentro de um bloco ou declarações múltiplas) */
NoAST* ligar_nos(NoAST* atual, NoAST* proximo) {
    if (!atual) return proximo;
    NoAST* temp = atual;
    while (temp->proximo != NULL) {
        temp = temp->proximo;
    }
    temp->proximo = proximo;
    return atual;
}
