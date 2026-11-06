#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "interpretador.h"
#include "parser.tab.h"   /* tokens dos operadores (PLUS, ...) e dos tipos (INT, ...) */

/* Erro de execução: mostra a mensagem e encerra o programa */
static void erro_execucao(const char* formato, ...) __attribute__((noreturn));
static void erro_execucao(const char* formato, ...) {
    va_list args;
    va_start(args, formato);
    fprintf(stderr, "Erro de execucao: ");
    vfprintf(stderr, formato, args);
    fprintf(stderr, "\n");
    va_end(args);
    exit(1);
}

static Valor avaliar_chamada(const NoAST* no);
static void interpretar_lista(const NoAST* no);

static TipoValor tipo_valor_do_token(int token) {
    switch (token) {
        case FLOAT: return VAL_FLOAT;
        case CHAR:  return VAL_CHAR;
        default:    return VAL_INT;
    }
}

/* ===================== Expressões (#47) ===================== */

static Valor avaliar_unaria(const NoAST* no) {
    Valor v = avaliar_expressao(no->esq);
    switch (no->dado.op) {
        case NOT:
            return valor_int(!valor_verdadeiro(v));
        case MINUS:
            return v.tipo == VAL_FLOAT ? valor_float(-v.dado.f) : valor_int(-v.dado.i);
        default:
            erro_execucao("operador unario desconhecido.");
    }
}

static Valor avaliar_binaria(const NoAST* no) {
    int op = no->dado.op;

    /* && e || só avaliam o lado direito quando necessário (curto-circuito) */
    if (op == AND) {
        if (!valor_verdadeiro(avaliar_expressao(no->esq))) return valor_int(0);
        return valor_int(valor_verdadeiro(avaliar_expressao(no->dir)));
    }
    if (op == OR) {
        if (valor_verdadeiro(avaliar_expressao(no->esq))) return valor_int(1);
        return valor_int(valor_verdadeiro(avaliar_expressao(no->dir)));
    }

    Valor a = avaliar_expressao(no->esq);
    Valor b = avaliar_expressao(no->dir);

    /* Se algum operando é float, a conta é em float; senão (int/char) é em int */
    if (a.tipo == VAL_FLOAT || b.tipo == VAL_FLOAT) {
        float x = valor_como_float(a), y = valor_como_float(b);
        switch (op) {
            case PLUS:   return valor_float(x + y);
            case MINUS:  return valor_float(x - y);
            case TIMES:  return valor_float(x * y);
            case DIVIDE:
                if (y == 0) erro_execucao("divisao por zero.");
                return valor_float(x / y);
            case MOD:    erro_execucao("o operador %% exige operandos inteiros.");
            case EQ:     return valor_int(x == y);
            case NEQ:    return valor_int(x != y);
            case LT:     return valor_int(x < y);
            case GT:     return valor_int(x > y);
            case LE:     return valor_int(x <= y);
            case GE:     return valor_int(x >= y);
        }
    } else {
        int x = a.dado.i, y = b.dado.i;
        switch (op) {
            case PLUS:   return valor_int(x + y);
            case MINUS:  return valor_int(x - y);
            case TIMES:  return valor_int(x * y);
            case DIVIDE:
                if (y == 0) erro_execucao("divisao por zero.");
                return valor_int(x / y);
            case MOD:
                if (y == 0) erro_execucao("divisao por zero.");
                return valor_int(x % y);
            case EQ:     return valor_int(x == y);
            case NEQ:    return valor_int(x != y);
            case LT:     return valor_int(x < y);
            case GT:     return valor_int(x > y);
            case LE:     return valor_int(x <= y);
            case GE:     return valor_int(x >= y);
        }
    }
    erro_execucao("operador binario desconhecido.");
}

/* ===================== Atribuição e acesso a variáveis (#48) ===================== */

static Valor avaliar_atribuicao(const NoAST* no) {
    if (no->esq == NULL || no->esq->tipo != NO_ID) {
        erro_execucao("o alvo de uma atribuicao precisa ser uma variavel.");
    }
    Valor novo = avaliar_expressao(no->dir);
    Valor gravado;
    if (!ambiente_escrever(no->esq->dado.string_val, novo, &gravado)) {
        erro_execucao("variavel '%s' nao declarada.", no->esq->dado.string_val);
    }
    return gravado;   /* o valor de "x = e" é o valor gravado em x */
}

Valor avaliar_expressao(const NoAST* no) {
    if (no == NULL) erro_execucao("expressao ausente.");

    switch (no->tipo) {
        case NO_LITERAL_INT:   return valor_int(no->dado.valor_int);
        case NO_LITERAL_FLOAT: return valor_float(no->dado.valor_float);
        case NO_LITERAL_CHAR:  return valor_char((char)no->dado.valor_int);
        case NO_ID: {
            Valor v;
            if (!ambiente_ler(no->dado.string_val, &v)) {
                erro_execucao("variavel '%s' nao declarada.", no->dado.string_val);
            }
            return v;
        }
        case NO_OP_UNARIA:     return avaliar_unaria(no);
        case NO_OP_BINARIA:    return avaliar_binaria(no);
        case NO_ATRIBUICAO:    return avaliar_atribuicao(no);
        case NO_LITERAL_STRING:
            erro_execucao("strings ainda nao sao suportadas em expressoes.");
        case NO_CHAMADA:       return avaliar_chamada(no);
        default:
            erro_execucao("o no da AST nao e uma expressao.");
    }
}

/* ===================== Funções (#51) ===================== */

#define PROFUNDIDADE_MAXIMA 1000   /* evita estourar a pilha do C em recursão infinita */

/* Funções definidas no programa: nome -> nó NO_FUNCAO da AST */
typedef struct FuncaoReg {
    const NoAST* no;
    struct FuncaoReg* proximo;
} FuncaoReg;

static FuncaoReg* funcoes = NULL;

/* Estado da chamada em andamento */
static const NoAST* funcao_atual = NULL;   /* função sendo executada (NULL = código do topo) */
static int   retornando = 0;               /* 1 = um 'return' foi executado: desfaz os comandos pendentes */
static Valor valor_retorno;                /* valor do 'return' */
static int   profundidade = 0;             /* chamadas aninhadas no momento */

static int contar_nos(const NoAST* lista) {
    int n = 0;
    for (; lista != NULL; lista = lista->proximo) n++;
    return n;
}

/* Registra todas as funções do topo, para poderem ser chamadas pelo nome */
static void registrar_funcoes(const NoAST* lista) {
    while (funcoes != NULL) {
        FuncaoReg* proximo = funcoes->proximo;
        free(funcoes);
        funcoes = proximo;
    }
    for (; lista != NULL; lista = lista->proximo) {
        if (lista->tipo != NO_FUNCAO) continue;
        FuncaoReg* r = (FuncaoReg*)malloc(sizeof(FuncaoReg));
        if (!r) erro_execucao("falha na alocacao de memoria.");
        r->no = lista;
        r->proximo = funcoes;
        funcoes = r;
    }
}

static const NoAST* buscar_funcao(const char* nome) {
    for (FuncaoReg* r = funcoes; r != NULL; r = r->proximo) {
        if (strcmp(r->no->dado.string_val, nome) == 0) return r->no;
    }
    return NULL;
}

/* Executa o corpo de 'f' num ambiente novo, com os argumentos já avaliados (passagem por valor) */
static Valor executar_funcao(const NoAST* f, const Valor* args) {
    const char* nome = f->dado.string_val;
    if (profundidade >= PROFUNDIDADE_MAXIMA) {
        erro_execucao("limite de recursao excedido em '%s' (%d chamadas aninhadas).",
                      nome, PROFUNDIDADE_MAXIMA);
    }

    const NoAST* funcao_anterior = funcao_atual;
    funcao_atual = f;
    profundidade++;

    /* Novo escopo-base: só enxerga o global, não as variáveis de quem chamou */
    ambiente_entrar_funcao();

    /* Cada parâmetro vira uma variável local com uma CÓPIA do argumento, convertida ao tipo declarado */
    int i = 0;
    for (const NoAST* p = f->esq; p != NULL; p = p->proximo, i++) {
        TipoValor tipo = tipo_valor_do_token(p->dado.tipo_dado);
        ambiente_declarar(p->esq->dado.string_val, converter_valor(args[i], tipo));
    }

    /* Os comandos do corpo rodam direto no escopo dos parâmetros (como na tabela de símbolos) */
    retornando = 0;
    interpretar_lista(f->dir ? f->dir->esq : NULL);

    /* Sem 'return' a função devolve 0 do tipo declarado */
    Valor resultado = retornando
        ? valor_retorno
        : converter_valor(valor_int(0), tipo_valor_do_token(f->tipo_retorno));
    retornando = 0;

    ambiente_sair_escopo();
    profundidade--;
    funcao_atual = funcao_anterior;
    return resultado;
}

/* Chamada "f(a, b)": avalia os argumentos no ambiente de quem chama e executa a função */
static Valor avaliar_chamada(const NoAST* no) {
    const char* nome = no->dado.string_val;
    const NoAST* f = buscar_funcao(nome);
    if (f == NULL) erro_execucao("funcao '%s' nao definida.", nome);

    int n_params = contar_nos(f->esq);
    int n_args   = contar_nos(no->esq);
    if (n_params != n_args) {
        erro_execucao("funcao '%s' espera %d argumento(s), recebeu %d.", nome, n_params, n_args);
    }

    /* Todos os argumentos são calculados ANTES de entrar no novo ambiente */
    Valor* args = n_args > 0 ? (Valor*)malloc(n_args * sizeof(Valor)) : NULL;
    if (n_args > 0 && !args) erro_execucao("falha na alocacao de memoria.");
    int i = 0;
    for (const NoAST* a = no->esq; a != NULL; a = a->proximo) {
        args[i++] = avaliar_expressao(a);
    }

    Valor resultado = executar_funcao(f, args);
    free(args);
    return resultado;
}

/* ===================== Comandos ===================== */

/* Para assim que um 'return' é executado: os comandos seguintes não rodam */
static void interpretar_lista(const NoAST* no) {
    for (; no != NULL && !retornando; no = no->proximo) {
        interpretar_comando(no);
    }
}

void interpretar_comando(const NoAST* no) {
    if (no == NULL) return;   /* comando vazio, ex: "if (x) ;" */
    switch (no->tipo) {
        case NO_DECLARACAO: {
            /* Sem inicializador, a variável começa com 0 */
            TipoValor tipo = tipo_valor_do_token(no->dado.tipo_dado);
            Valor inicial = no->dir ? avaliar_expressao(no->dir) : valor_int(0);
            ambiente_declarar(no->esq->dado.string_val, converter_valor(inicial, tipo));
            break;
        }
        case NO_BLOCO:
            ambiente_entrar_escopo();
            interpretar_lista(no->esq);
            ambiente_sair_escopo();
            break;
        case NO_FUNCAO:
            break;   /* definir uma função não executa nada; chamadas ficam para depois */
        case NO_IF:
            /* esq = condicao, dir = comando do if, centro = comando do else (ou NULL) */
            if (valor_verdadeiro(avaliar_expressao(no->esq))) {
                interpretar_comando(no->dir);
            } else if (no->centro != NULL) {
                interpretar_comando(no->centro);
            }
            break;
        case NO_WHILE:
            /* esq = condicao, dir = corpo; a condicao e reavaliada a cada volta */
            while (!retornando && valor_verdadeiro(avaliar_expressao(no->esq))) {
                interpretar_comando(no->dir);
            }
            break;
        case NO_RETURN:
            if (funcao_atual == NULL) erro_execucao("'return' fora de uma funcao.");
            /* O valor é convertido para o tipo de retorno da função; 'retornando' interrompe o resto */
            valor_retorno = converter_valor(avaliar_expressao(no->esq),
                                            tipo_valor_do_token(funcao_atual->tipo_retorno));
            retornando = 1;
            break;
        default:
            avaliar_expressao(no);   /* comando de expressão: "x = x + 1;" */
            break;
    }
}

void executar_programa(const NoAST* raiz) {
    ambiente_iniciar();
    registrar_funcoes(raiz);
    interpretar_lista(raiz);

    printf("\n");
    ambiente_imprimir_global();
}
