# AST e Tabela de Símbolos

Este documento descreve as duas estruturas de dados que o parser monta durante a análise sintática do MiniC: a **AST** (árvore sintática abstrata) e a **tabela de símbolos** com escopos aninhados.

| Arquivo | Conteúdo |
|---|---|
| `src/ast.h` / `src/ast.c` | Estrutura da AST, funções construtoras, `ligar_nos` e `imprimir_ast` |
| `src/tabela.h` / `src/tabela.c` | Tabela de símbolos com pilha de escopos |
| `parser/parser.y` | Onde as duas são usadas: as ações das regras constroem a AST e alimentam a tabela |

---

## 1. AST

### 1.1 O nó

Todo nó é um `NoAST`, com um tipo, quatro ponteiros e um campo de dado:

| Campo | Uso |
|---|---|
| `tipo` | Um valor do enum `TipoNo`, que diz o que o nó representa |
| `esq`, `dir`, `centro` | Filhos do nó. O significado de cada um depende do tipo (tabela abaixo) |
| `proximo` | Encadeamento horizontal: liga comandos (ou declarações) que vêm em sequência |
| `dado` | `union` com o valor específico do nó: `valor_int`, `valor_float`, `string_val`, `op` ou `tipo_dado` |

### 1.2 Tipos de nó

| `TipoNo` | Representa | `dado` | `esq` | `dir` | `centro` |
|---|---|---|---|---|---|
| `NO_LITERAL_INT` | Inteiro (`42`) | `valor_int` | | | |
| `NO_LITERAL_FLOAT` | Float (`3.14`) | `valor_float` | | | |
| `NO_LITERAL_CHAR` | Char (`'a'`) | `valor_int` (o char guardado como int) | | | |
| `NO_LITERAL_STRING` | String (`"oi"`) | `string_val` (sem as aspas) | | | |
| `NO_ID` | Nome de variável | `string_val` | | | |
| `NO_OP_BINARIA` | `a + b`, `a && b`, ... | `op` (token do Bison) | operando esquerdo | operando direito | |
| `NO_OP_UNARIA` | `-a`, `!a` | `op` | operando | | |
| `NO_ATRIBUICAO` | `x = expr` | | alvo da atribuição | valor | |
| `NO_DECLARACAO` | `int x;` / `int x = expr;` | `tipo_dado` (`INT`, `FLOAT` ou `CHAR`) | nó `NO_ID` | valor inicial (`NULL` se não houver) | |
| `NO_IF` | `if` / `if-else` | | condição | bloco do `if` | bloco do `else` (`NULL` se não houver) |
| `NO_WHILE` | `while` | | condição | corpo | |
| `NO_BLOCO` | `{ ... }` | | primeiro comando da lista | | |
| `NO_RETURN` | `return expr;` | | expressão | | |
| `NO_FUNCAO` | Função | `tipo_dado` e `string_val` | parâmetros | corpo | |
| `NO_PROGRAMA` | Reservado | | | | |

`NO_FUNCAO` e `NO_PROGRAMA` ainda não são gerados pelo parser, pois a gramática atual não aceita funções.

### 1.3 Encadeamento com `proximo`

Uma sequência de comandos não é uma árvore de listas: é uma lista encadeada pelo campo `proximo`. A função `ligar_nos(atual, proximo)` percorre `atual` até o fim da cadeia e pendura `proximo` lá. Se `atual` for `NULL`, devolve `proximo`.

Isso é usado em dois lugares:

- **`lista_comandos`**: os comandos de um programa ou de um bloco ficam encadeados.
- **Declaração múltipla**: `int a, b = 1;` gera dois nós `NO_DECLARACAO` ligados por `proximo`, ambos com o mesmo `tipo_dado`.

A raiz da árvore (`raiz_ast`, definida em `parser.y`) é o **primeiro comando do programa**. Os demais são alcançados seguindo `proximo`.

### 1.4 Funções construtoras

| Função | Cria |
|---|---|
| `criar_no_literal_int(int)` | `NO_LITERAL_INT` |
| `criar_no_literal_float(float)` | `NO_LITERAL_FLOAT` |
| `criar_no_literal_char(char)` | `NO_LITERAL_CHAR` |
| `criar_no_literal_string(const char*)` | `NO_LITERAL_STRING` (copia a string com `strdup`) |
| `criar_no_id(const char*)` | `NO_ID` (copia o nome com `strdup`) |
| `criar_no_op_binaria(op, esq, dir)` | `NO_OP_BINARIA` |
| `criar_no_op_unaria(op, esq)` | `NO_OP_UNARIA` |
| `criar_no_atribuicao(id, expressao)` | `NO_ATRIBUICAO` |
| `criar_no_declaracao(tipo_dado, id, expressao)` | `NO_DECLARACAO` |
| `criar_no_if(cond, bloco_if, bloco_else)` | `NO_IF` |
| `criar_no_while(cond, bloco)` | `NO_WHILE` |
| `criar_no_bloco(lista_comandos)` | `NO_BLOCO` |
| `criar_no_return(expressao)` | `NO_RETURN` |
| `criar_no_funcao(tipo, nome, parametros, bloco)` | `NO_FUNCAO` |
| `ligar_nos(atual, proximo)` | Encadeia dois nós ou duas cadeias |

Todas alocam o nó com `malloc` e começam com os quatro ponteiros em `NULL`. A AST ainda não é liberada ao final do programa.

### 1.5 Impressão

`imprimir_ast(raiz)` percorre a árvore e imprime um nó por linha, com dois espaços de indentação por nível. Para cada nó, imprime primeiro os filhos na ordem `esq`, `dir`, `centro` e depois segue para o próximo irmão.

Por isso, num `if`, a ordem dos filhos é: **condição, bloco do `if`, bloco do `else`**. Numa declaração, o primeiro filho é o `ID` e o segundo, se existir, é o valor inicial.

A função é chamada no `main` somente quando o `yyparse` termina com sucesso.

Exemplo, para `int x = 2 + 3 * 4;` seguido de `if (x > 5) { x = 1; } else x = 0;`:

```
Programa
  Declaracao int
    ID x
    Op +
      Int 2
      Op *
        Int 3
        Int 4
  If
    Op >
      ID x
      Int 5
    Bloco
      Atribuicao
        ID x
        Int 1
    Atribuicao
      ID x
      Int 0
```

---

## 2. Tabela de símbolos

### 2.1 Estruturas

```c
typedef struct Simbolo {
    char* nome;
    int tipo_dado;           /* INT, FLOAT ou CHAR (tokens do Bison) */
    struct Simbolo* proximo; /* próximo símbolo do mesmo escopo */
} Simbolo;

typedef struct Escopo {
    Simbolo* tabela;         /* lista encadeada de símbolos deste escopo */
    struct Escopo* pai;      /* escopo externo */
} Escopo;
```

Os escopos formam uma **pilha** implementada como lista encadeada pelo campo `pai`. A variável global `escopo_atual` aponta para o topo, que é o escopo mais interno no momento.

### 2.2 Funções

| Função | O que faz |
|---|---|
| `inicializar_tabela()` | Cria o escopo global. Deve ser chamada uma vez, antes do `yyparse` |
| `entrar_escopo()` | Empilha um novo escopo vazio |
| `sair_escopo()` | Desempilha o escopo atual e libera todos os seus símbolos |
| `inserir_variavel(nome, tipo)` | Insere no escopo atual. Retorna `1` em caso de sucesso e `0` se o nome já existe **no mesmo escopo** (ou se a tabela não foi inicializada) |
| `buscar_variavel(nome)` | Procura do escopo atual para fora, até o global. Retorna o `Simbolo*` ou `NULL` |

### 2.3 Regras de escopo

- Um nome só pode ser declarado uma vez por escopo.
- Um escopo interno pode **redeclarar** um nome de um escopo externo (sombreamento). Por exemplo, `int x; { int x; }` é válido.
- Cada bloco `{ ... }` cria um escopo novo. `if` e `while` sem chaves não criam escopo.

### 2.4 Integração com o parser

Em `parser.y`:

- **`main`** chama `inicializar_tabela()` antes de `yyparse()`.
- **`bloco_inicio: LBRACE`** chama `entrar_escopo()` e **`bloco_fim: RBRACE`** chama `sair_escopo()`.
- **`tipo`** (`INT`, `FLOAT`, `CHAR`) guarda o tipo na variável global `tipo_atual`. Assim, numa declaração múltipla como `int a, b;`, todos os nomes da lista recebem o mesmo tipo.
- **`declaracao_variavel`** (`ID` ou `ID ASSIGN expressao`) chama `inserir_variavel` com `tipo_atual`. Toda declaração passa por essa regra, via `declaracao → tipo lista_declaracoes SEMI`.

Se a inserção falhar, o parser imprime em `stderr`:

```
Erro Semantico na linha N: Variavel 'x' ja declarada neste escopo.
```

O parser **continua** depois desse erro, e a análise ainda termina com `Analise sintatica concluida com sucesso!`, já que o erro é semântico e não sintático.

---

## 3. Limitações atuais

- `buscar_variavel` existe, mas o parser ainda não a chama. **O uso de variável não declarada não é detectado.**
- Erros semânticos não alteram o retorno do `yyparse`, então o programa não indica falha por causa deles.
- Não há funções na gramática. Em `criar_no_funcao`, `tipo_dado` e `string_val` pertencem à mesma `union` e se sobrescrevem; isso precisa ser corrigido (por exemplo, guardando o tipo em outro campo) quando funções forem implementadas.
- A AST e o escopo global não são liberados ao final da execução.

## 4. Como testar

```bash
make clean && make
printf 'int x = 2 + 3 * 4;\nif (x > 5) { x = 1; } else x = 0;\n' | ./build/compilador   # imprime a árvore
printf 'int x;\nint x;\n' | ./build/compilador                                            # erro semântico (redeclaração)
printf 'int x;\n{ int x; }\n' | ./build/compilador                                        # válido (sombreamento)
printf 'int x = 1\nint y;\n' | ./build/compilador                                         # erro sintático, sem árvore
```