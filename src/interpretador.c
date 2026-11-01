#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
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
        case NO_CHAMADA:
            erro_execucao("chamada de funcao ainda nao suportada.");
        default:
            erro_execucao("o no da AST nao e uma expressao.");
    }
}

/* ===================== Comandos ===================== */

static void interpretar_lista(const NoAST* no) {
    for (; no != NULL; no = no->proximo) {
        interpretar_comando(no);
    }
}

void interpretar_comando(const NoAST* no) {
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
        case NO_WHILE:
        case NO_RETURN:
            erro_execucao("este comando ainda nao e suportado pelo interpretador.");
        default:
            avaliar_expressao(no);   /* comando de expressão: "x = x + 1;" */
            break;
    }
}

void executar_programa(const NoAST* raiz) {
    ambiente_iniciar();
    interpretar_lista(raiz);
    printf("\n");
    ambiente_imprimir_global();
}