#ifndef AST_H
#define AST_H

/* Enumeração de todos os tipos de nós que compõem o nosso Mini-C */
typedef enum {
    NO_LITERAL_INT,
    NO_LITERAL_FLOAT,
    NO_LITERAL_CHAR,
    NO_LITERAL_STRING,
    NO_ID,
    NO_OP_BINARIA,
    NO_OP_UNARIA,
    NO_ATRIBUICAO,
    NO_DECLARACAO,
    NO_IF,
    NO_WHILE,
    NO_BLOCO,
    NO_RETURN,
    NO_FUNCAO,      // Preparado para expansão futura
    NO_PROGRAMA
} TipoNo;

/* Estrutura genérica do Nó da AST */
typedef struct NoAST {
    TipoNo tipo;

    // Ponteiros para estruturação da árvore e sub-blocos
    struct NoAST* esq;      // Filho esquerdo (ex: operando esquerdo, condição)
    struct NoAST* dir;      // Filho direito (ex: operando direito, bloco if)
    struct NoAST* centro;   // Filho central (ex: bloco else)
    struct NoAST* proximo;  // Encadeamento horizontal (ex: lista de comandos ou declarações)

    // Armazenamento de dados específicos baseado no tipo do nó
    union {
        int valor_int;
        float valor_float;
        char* string_val; // Guarda nomes de variáveis (ID) ou literais de texto
        int op;           // Guarda o token do operador (PLUS, MINUS, EQ, etc)
        int tipo_dado;    // Guarda o tipo primitivo (INT, FLOAT, CHAR)
    } dado;

} NoAST;

/* Protótipos das funções construtoras da issue #27 */
NoAST* criar_no_literal_int(int valor);
NoAST* criar_no_literal_float(float valor);
NoAST* criar_no_literal_char(char valor);
NoAST* criar_no_literal_string(const char* valor);
NoAST* criar_no_id(const char* nome);

NoAST* criar_no_op_binaria(int op, NoAST* esq, NoAST* dir);
NoAST* criar_no_op_unaria(int op, NoAST* esq);
NoAST* criar_no_atribuicao(NoAST* id, NoAST* expressao);

NoAST* criar_no_declaracao(int tipo_dado, NoAST* id, NoAST* expressao);
NoAST* criar_no_if(NoAST* condicao, NoAST* bloco_if, NoAST* bloco_else);
NoAST* criar_no_while(NoAST* condicao, NoAST* bloco);
NoAST* criar_no_bloco(NoAST* lista_comandos);
NoAST* criar_no_return(NoAST* expressao);
NoAST* criar_no_funcao(int tipo_retorno, const char* nome, NoAST* parametros, NoAST* bloco);

NoAST* ligar_nos(NoAST* atual, NoAST* proximo);

#endif
