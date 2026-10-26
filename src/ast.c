#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "ast.h"
#include "tabela.h"
#include "parser.tab.h"   /* nomes dos tokens (PLUS, INT, ...) para imprimir */

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
    no->tipo_retorno = 0;
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
    no->tipo_retorno = tipo_retorno;   // fora da union, para não colidir com o nome
    no->dado.string_val = strdup(nome);
    no->esq = parametros;
    no->dir = bloco;
    return no;
}

NoAST* criar_no_chamada(const char* nome, NoAST* argumentos) {
    NoAST* no = alocar_no(NO_CHAMADA);
    no->dado.string_val = strdup(nome);
    no->esq = argumentos;   // argumentos encadeados por 'proximo'; pode ser NULL
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

/* ===================== Impressao da AST (issue #33) ===================== */

/* Mesma ordem do enum TipoNo em ast.h */
static const char* nomes_no[] = {
    "Int", "Float", "Char", "String", "ID", "Op", "OpUnaria", "Atribuicao",
    "Declaracao", "If", "While", "Bloco", "Return", "Funcao", "Programa", "Chamada"
};

const char* nome_token(int t) {
    switch (t) {
        case PLUS: return "+";   case MINUS: return "-";  case TIMES: return "*";
        case DIVIDE: return "/"; case MOD: return "%";    case EQ: return "==";
        case NEQ: return "!=";   case LT: return "<";     case GT: return ">";
        case LE: return "<=";    case GE: return ">=";    case AND: return "&&";
        case OR: return "||";    case NOT: return "!";
        case INT: return "int";  case FLOAT: return "float"; case CHAR: return "char";
        default: return "?";
    }
}

/* Imprime o no, seus filhos (esq, dir, centro) e depois os irmaos (proximo) */
static void imprimir_no(const NoAST* no, int nivel) {
    for (; no != NULL; no = no->proximo) {
        printf("%*s%s", nivel * 2, "", nomes_no[no->tipo]);
        switch (no->tipo) {
            case NO_LITERAL_INT:    printf(" %d", no->dado.valor_int); break;
            case NO_LITERAL_FLOAT:  printf(" %g", no->dado.valor_float); break;
            case NO_LITERAL_CHAR:   printf(" '%c'", no->dado.valor_int); break;
            case NO_LITERAL_STRING:
            case NO_ID:
            case NO_CHAMADA:        printf(" %s", no->dado.string_val); break;
            case NO_FUNCAO:         printf(" %s %s", nome_token(no->tipo_retorno), no->dado.string_val); break;
            case NO_OP_BINARIA:
            case NO_OP_UNARIA:      printf(" %s", nome_token(no->dado.op)); break;
            case NO_DECLARACAO:     printf(" %s", nome_token(no->dado.tipo_dado)); break;
            default: break;
        }
        printf("\n");
        imprimir_no(no->esq, nivel + 1);
        imprimir_no(no->dir, nivel + 1);
        imprimir_no(no->centro, nivel + 1);   /* else, quando existir */
    }
}

void imprimir_ast(const NoAST* raiz) {
    printf("Programa\n");
    imprimir_no(raiz, 1);
}

/* ===================== Inferencia de tipos ===================== */

int tipo_da_expressao(const NoAST* no) {
    if (!no) return -1;

    switch (no->tipo) {
        case NO_LITERAL_INT:    return INT;
        case NO_LITERAL_FLOAT:  return FLOAT;
        case NO_LITERAL_CHAR:   return CHAR;
        case NO_LITERAL_STRING: return TIPO_STRING;

        case NO_ID:
        case NO_CHAMADA: {
            Simbolo* s = buscar_variavel(no->dado.string_val);
            return s ? s->tipo_dado : -1;
        }

        case NO_ATRIBUICAO:
            return tipo_da_expressao(no->dir);

        case NO_OP_UNARIA:
            if (no->dado.op == NOT) return INT; /* resultado logico */
            return tipo_da_expressao(no->esq);  /* MINUS preserva o tipo do operando */

        case NO_OP_BINARIA: {
            int op = no->dado.op;
            /* operadores relacionais/logicos sempre resultam em int (0 ou 1) */
            if (op == EQ || op == NEQ || op == LT || op == GT || op == LE || op == GE
                || op == AND || op == OR) {
                return INT;
            }
            /* aritmeticos: promove para float se qualquer lado for float */
            int te = tipo_da_expressao(no->esq);
            int td = tipo_da_expressao(no->dir);
            if (te == FLOAT || td == FLOAT) return FLOAT;
            return INT;
        }

        default:
            return -1;
    }
}

int tipos_compativeis(int esperado, int obtido) {
    if (esperado == obtido) return 1;
    if (esperado == FLOAT && (obtido == INT || obtido == CHAR)) return 1;
    if (esperado == INT && obtido == CHAR) return 1;
    return 0;
}