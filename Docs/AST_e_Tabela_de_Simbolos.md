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
| `NO_FUNCAO` | Definição de função | `string_val` (nome) + `tipo_retorno` (campo proprio, fora da union) | parâmetros | corpo (`NO_BLOCO`) | |
| `NO_CHAMADA` | Chamada de função (`f(1, 2)`) | `string_val` (nome) | lista de argumentos (`NO_ID`/expressões ligados por `proximo`) | | |
| `NO_PROGRAMA` | Reservado, não utilizado atualmente | | | | |

`NO_FUNCAO` é gerado por `definicao_funcao` no parser, e usa o campo `tipo_retorno` (fora da `union dado`) justamente para não colidir com `string_val` do nome da função — os dois são usados ao mesmo tempo. `NO_CHAMADA` é gerado sempre que uma chamada de função aparece numa expressão, após passar pela validação semântica de `validar_chamada` (ver Seção 2.5).

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
| `criar_no_chamada(nome, argumentos)` | `NO_CHAMADA` |
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
    int tipo_dado;           /* variável: seu tipo | função: tipo de retorno (INT, FLOAT ou CHAR) */
    int eh_funcao;           /* 1 se for função, 0 se for variável */
    int num_params;          /* só para função: quantidade de parâmetros */
    int* tipos_params;       /* só para função: tipo de cada parâmetro (NULL se não houver) */
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

### 2.5 Análise semântica

Além da tabela de símbolos e dos escopos, o parser faz três verificações semânticas, todas implementadas em `parser.y` reaproveitando duas funções auxiliares de `ast.c`:

- **`tipo_da_expressao(no)`**: infere o tipo (`INT`, `FLOAT`, `CHAR` ou `TIPO_STRING`) resultante de uma expressão, percorrendo a AST recursivamente. Retorna `-1` quando o tipo não pode ser determinado (ex: identificador não declarado — nesse caso o próprio erro de "não declarada" já foi reportado em outro ponto, então o chamador deve ignorar o `-1` sem gerar um segundo erro).
- **`tipos_compativeis(esperado, obtido)`**: decide se um valor do tipo `obtido` pode ser usado onde se espera `esperado`, seguindo a Seção 6.1 da Especificação: `int` e `float` convertem implicitamente entre si (nos dois sentidos — `float → int` trunca), e `char` nunca converte implicitamente com `int`/`float`.

As três verificações:

1. **Tipo de retorno (`comando: RETURN expressao SEMI`)**: compara o tipo da expressão com `tipo_retorno_atual`, uma variável global atualizada em `iniciar_funcao` toda vez que o parser entra numa nova definição de função. Também detecta `return` fora de qualquer função.
2. **Atribuição e inicialização**: tanto `expressao: expressao ASSIGN expressao` quanto `declaracao_variavel: ID ASSIGN expressao` comparam o tipo do valor atribuído com o tipo do alvo.
3. **Chamada de função (`validar_chamada`)**: antes de construir o nó `NO_CHAMADA`, verifica, nesta ordem: (a) se a função existe na tabela de símbolos; (b) se o símbolo encontrado é de fato uma função (`eh_funcao`) e não uma variável; (c) se o número de argumentos bate com `num_params`; (d) se cada argumento é compatível com o tipo do parâmetro correspondente. Qualquer erro é reportado em `stderr`, mas o nó da chamada é construído normalmente mesmo assim, para o parser continuar com segurança.

Operadores binários e unários também passam por uma checagem mais simples: `criar_binaria_verificada`/`criar_unaria_verificada` rejeitam qualquer operando do tipo `TIPO_STRING` (string literal), já que strings só podem ser usadas como argumento direto de uma chamada (ex: `printf`), nunca dentro de uma expressão aritmética/lógica.

---

## 3. Limitações atuais

- A AST e o escopo global não são liberados ao final da execução.
- `return` dentro de uma função não é obrigatório: uma função `int` pode nunca executar `return` e o parser não acusa erro (a Especificação, Seção 11 item 7, pede essa verificação; ainda não implementada).
- A verificação de tipos opera sobre o tipo *declarado* das variáveis e literais; não há execução real, então nenhum valor é de fato calculado ou convertido em tempo de análise.
- Não há `void`: toda função precisa declarar um tipo de retorno `int`, `float` ou `char`.

## 4. Como testar

```bash
make clean && make
printf 'int main() { int x = 2 + 3 * 4; if (x > 5) { x = 1; } else x = 0; return 0; }\n' | ./build/compilador   # imprime a árvore
printf 'int main() { int x; int x; return 0; }\n' | ./build/compilador                                          # erro semântico (redeclaração)
printf 'int main() { int x; { int x; } return 0; }\n' | ./build/compilador                                      # válido (sombreamento)
printf 'int f() { return 3.14; } int main() { return 0; }\n' | ./build/compilador                                 # ok, float->int trunca (Secao 6.1)
printf "int f() { return 'a'; } int main() { return 0; }\n" | ./build/compilador                                  # erro semantico (char->int nao permitido)
printf 'int soma(int a, int b) { return a + b; } int main() { int x = soma(1); return 0; }\n' | ./build/compilador # erro semantico (aridade)
```