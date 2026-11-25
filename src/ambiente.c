#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ambiente.h"

/* ===================== Valores ===================== */

Valor valor_int(int i)     { Valor v; v.tipo = VAL_INT;   v.dado.i = i; return v; }
Valor valor_float(float f) { Valor v; v.tipo = VAL_FLOAT; v.dado.f = f; return v; }
Valor valor_char(char c)   { Valor v; v.tipo = VAL_CHAR;  v.dado.i = c; return v; }

int valor_como_int(Valor v) {
    return v.tipo == VAL_FLOAT ? (int)v.dado.f : v.dado.i;
}

float valor_como_float(Valor v) {
    return v.tipo == VAL_FLOAT ? v.dado.f : (float)v.dado.i;
}

int valor_verdadeiro(Valor v) {
    return v.tipo == VAL_FLOAT ? v.dado.f != 0 : v.dado.i != 0;
}

Valor converter_valor(Valor v, TipoValor destino) {
    switch (destino) {
        case VAL_FLOAT: return valor_float(valor_como_float(v));
        case VAL_CHAR:  return valor_char((char)valor_como_int(v));
        default:        return valor_int(valor_como_int(v));
    }
}

/* ===================== Ambiente ===================== */

static EscopoExec* exec_atual  = NULL;   /* topo da pilha */
static EscopoExec* exec_global = NULL;   /* base da pilha */

static void* alocar(size_t tamanho) {
    void* p = malloc(tamanho);
    if (!p) {
        fprintf(stderr, "Erro fatal: Falha na alocacao de memoria do ambiente.\n");
        exit(1);
    }
    return p;
}

static void empilhar(int eh_funcao) {
    EscopoExec* e = (EscopoExec*)alocar(sizeof(EscopoExec));
    e->vars = NULL;
    e->pai = exec_atual;
    e->eh_funcao = eh_funcao;
    exec_atual = e;
}

void ambiente_iniciar(void) {
    exec_atual = NULL;
    empilhar(0);
    exec_global = exec_atual;
}

void ambiente_entrar_escopo(void) { empilhar(0); }
void ambiente_entrar_funcao(void) { empilhar(1); }

void ambiente_sair_escopo(void) {
    if (exec_atual == NULL || exec_atual == exec_global) return;   /* o global nunca sai */

    EscopoExec* removido = exec_atual;
    exec_atual = removido->pai;

    VarExec* v = removido->vars;
    while (v != NULL) {
        VarExec* proximo = v->proximo;
        free(v->nome);
        free(v);
        v = proximo;
    }
    free(removido);
}

int ambiente_declarar(const char* nome, Valor inicial) {
    if (exec_atual == NULL) return 0;

    for (VarExec* v = exec_atual->vars; v != NULL; v = v->proximo) {
        if (strcmp(v->nome, nome) == 0) return 0;   /* já existe neste escopo */
    }

    VarExec* nova = (VarExec*)alocar(sizeof(VarExec));
    nova->nome = strdup(nome);
    nova->valor = inicial;
    nova->proximo = exec_atual->vars;
    exec_atual->vars = nova;
    return 1;
}

/* Busca de dentro para fora. Ao sair do escopo-base de uma função, salta direto
   para o global: as variáveis locais de quem chamou não são visíveis. */
VarExec* ambiente_buscar(const char* nome) {
    EscopoExec* e = exec_atual;
    while (e != NULL) {
        for (VarExec* v = e->vars; v != NULL; v = v->proximo) {
            if (strcmp(v->nome, nome) == 0) return v;
        }
        e = e->eh_funcao ? exec_global : e->pai;
    }
    return NULL;
}

int ambiente_ler(const char* nome, Valor* saida) {
    VarExec* v = ambiente_buscar(nome);
    if (!v) return 0;
    *saida = v->valor;
    return 1;
}

int ambiente_escrever(const char* nome, Valor novo, Valor* gravado) {
    VarExec* v = ambiente_buscar(nome);
    if (!v) return 0;
    v->valor = converter_valor(novo, v->valor.tipo);
    if (gravado) *gravado = v->valor;
    return 1;
}

/* ===================== Depuração ===================== */

static void imprimir_vars(const VarExec* v) {
    if (v == NULL) return;
    imprimir_vars(v->proximo);   /* a lista é invertida: imprime na ordem de declaração */
    switch (v->valor.tipo) {
        case VAL_FLOAT: printf("  %s = %g (float)\n", v->nome, v->valor.dado.f); break;
        case VAL_CHAR:  printf("  %s = '%c' (char)\n", v->nome, (char)v->valor.dado.i); break;
        default:        printf("  %s = %d (int)\n", v->nome, v->valor.dado.i); break;
    }
}

void ambiente_imprimir_global(void) {
    printf("Variaveis globais ao final da execucao:\n");
    if (exec_global == NULL || exec_global->vars == NULL) {
        printf("  (nenhuma)\n");
        return;
    }
    imprimir_vars(exec_global->vars);
}