# AST e Tabela de Símbolos

Este documento descreve as duas estruturas de dados que o parser monta durante a análise sintática do MiniC: a **AST** (árvore sintática abstrata) e a **tabela de símbolos** com escopos aninhados. Ele também resume as verificações semânticas feitas sobre elas.

| Arquivo | Conteúdo |
|---|---|
| `src/ast.h` / `src/ast.c` | Estrutura da AST, funções construtoras, `ligar_nos`, `imprimir_ast`, inferência de tipos |
| `src/tabela.h` / `src/tabela.c` | Tabela de símbolos com pilha de escopos |
| `parser/parser.y` | Onde as duas são usadas: as ações das regras constroem a AST, alimentam a tabela e fazem as checagens semânticas |

> A execução **não** usa a tabela de símbolos: ela tem um ambiente próprio (`src/ambiente.c`), com valores em vez de tipos. Os motivos estão em [decisoes-tecnicas.md](decisoes-tecnicas.md), D8.

---

## 1. AST

### 1.1 O nó

Todo nó é um `NoAST`, com um tipo, cinco ponteiros, um campo `tipo_retorno` e um campo de dado:

| Campo | Uso |
|---|---|
| `tipo` | Um valor do enum `TipoNo`, que diz o que o nó representa |
| `esq`, `dir`, `centro`, `extra` | Filhos do nó. O significado de cada um depende do tipo (tabela abaixo) |
| `proximo` | Encadeamento horizontal: liga comandos, declarações, parâmetros ou argumentos que vêm em sequência |
| `tipo_retorno` | Só em `NO_FUNCAO`: tipo de retorno (`INT`, `FLOAT` ou `CHAR`). Fica fora da `union` porque o nome da função já ocupa `string_val` |
| `dado` | `union` com o valor específico do nó: `valor_int`, `valor_float`, `string_val`, `op` ou `tipo_dado` |

### 1.2 Tipos de nó

| `TipoNo` | Representa | `dado` | `esq` | `dir` | `centro` | `extra` |
|---|---|---|---|---|---|---|
| `NO_LITERAL_INT` | Inteiro (`42`) | `valor_int` | | | | |
| `NO_LITERAL_FLOAT` | Float (`3.14`) | `valor_float` | | | | |
| `NO_LITERAL_CHAR` | Char (`'a'`) | `valor_int` (código do caractere) | | | | |
| `NO_LITERAL_STRING` | String (`"oi"`) | `string_val` (sem as aspas) | | | | |
| `NO_ID` | Nome de variável | `string_val` | | | | |
| `NO_OP_BINARIA` | `a + b`, `a && b`, ... | `op` (token do Bison) | operando esquerdo | operando direito | | |
| `NO_OP_UNARIA` | `-a`, `!a` | `op` | operando | | | |
| `NO_ATRIBUICAO` | `x = expr` (e `x++`, `--x`, ...) | | alvo | valor | | |
| `NO_DECLARACAO` | `int x;` / `int x = expr;` | `tipo_dado` (`INT`, `FLOAT` ou `CHAR`) | nó `NO_ID` | valor inicial (`NULL` se não houver) | | |
| `NO_IF` | `if` / `if-else` | | condição | comando do `if` | comando do `else` (`NULL` se não houver) | |
| `NO_WHILE` | `while` | | condição | corpo | | |
| `NO_FOR` | `for` | | inicialização | condição | passo | corpo |
| `NO_PRINTF` | `printf(...)` | `string_val` (formato) | lista de argumentos (`NULL` se não houver) | | | |
| `NO_SCANF` | `scanf(...)` | `string_val` (formato) | `NO_ID` da variável lida | | | |
| `NO_BLOCO` | `{ ... }` | | primeiro comando da lista (`NULL` se o bloco é vazio) | | | |
| `NO_RETURN` | `return expr;` | | expressão | | | |
| `NO_FUNCAO` | Definição de função | `string_val` (nome) | parâmetros (`NO_DECLARACAO` encadeados) | corpo (`NO_BLOCO`) | | |
| `NO_CHAMADA` | Chamada `f(1, 2)` | `string_val` (nome) | argumentos (encadeados por `proximo`) | | | |
| `NO_PROGRAMA` | Reservado, não utilizado | | | | | |

Observações:

- `x++`, `++x`, `x--` e `--x` **não têm nó próprio**: o parser os transforma em `NO_ATRIBUICAO` (`x = x + 1` ou `x = x - 1`). Ver [decisoes-tecnicas.md](decisoes-tecnicas.md), D13.
- `NO_FUNCAO` é gerado por `definicao_funcao` e `NO_CHAMADA` por toda chamada de função numa expressão, depois da validação de `validar_chamada` (Seção 2.5).
- Os filhos de `NO_IF`, `NO_WHILE` e `NO_FOR` que são "corpo" ou "ramo" são **comandos quaisquer**: um bloco ou um único comando sem chaves.

### 1.3 Encadeamento com `proximo`

Uma sequência de comandos não é uma árvore de listas: é uma lista encadeada pelo campo `proximo`. A função `ligar_nos(atual, proximo)` percorre `atual` até o fim da cadeia e pendura `proximo` lá. Se `atual` for `NULL`, devolve `proximo`.

Isso é usado em quatro lugares:

- **`lista_itens` e `lista_comandos`**: os itens do programa e os comandos de um bloco ficam encadeados.
- **Declaração múltipla**: `int a, b = 1;` gera dois nós `NO_DECLARACAO` ligados por `proximo`, ambos com o mesmo `tipo_dado`.
- **Parâmetros** de uma função e **argumentos** de uma chamada ou de um `printf`.

A raiz da árvore (`raiz_ast`, definida em `parser.y`) é o **primeiro item do programa** (função, declaração global ou comando). Os demais são alcançados seguindo `proximo`.

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
| `criar_no_for(init, cond, inc, corpo)` | `NO_FOR` |
| `criar_no_printf(formato, argumentos)` | `NO_PRINTF` (copia o formato com `strdup`) |
| `criar_no_scanf(formato, variavel)` | `NO_SCANF` (copia o formato com `strdup`) |
| `criar_no_bloco(lista_comandos)` | `NO_BLOCO` |
| `criar_no_return(expressao)` | `NO_RETURN` |
| `criar_no_funcao(tipo, nome, parametros, bloco)` | `NO_FUNCAO` |
| `criar_no_chamada(nome, argumentos)` | `NO_CHAMADA` |
| `ligar_nos(atual, proximo)` | Encadeia dois nós ou duas cadeias |

Todas alocam o nó com `malloc` e começam com os cinco ponteiros em `NULL`. A AST ainda não é liberada ao final do programa.

### 1.5 Impressão

`imprimir_ast(raiz)` percorre a árvore e imprime um nó por linha, com dois espaços de indentação por nível. Para cada nó, imprime primeiro os filhos na ordem `esq`, `dir`, `centro`, `extra` e depois segue para o próximo irmão. Caracteres de controle em literais `char` (`\n`, `\t`, `\0`) aparecem escapados, para a árvore ficar numa linha só.

Por isso, num `if`, a ordem dos filhos é: **condição, comando do `if`, comando do `else`**. No `for`, é: **inicialização, condição, passo, corpo**. Numa declaração, o primeiro filho é o `ID` e o segundo, se existir, é o valor inicial.

A impressão só acontece com a opção `-ast` (`./build/compilador -ast programa.c`) e apenas quando a análise termina **sem erros**. Com `-ast`, as variáveis globais também são listadas ao final da execução.

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

Os escopos formam uma **pilha** implementada como lista encadeada pelo campo `pai`. A variável global `escopo_atual` aponta para o topo, que é o escopo mais interno no momento. Funções e variáveis dividem o mesmo espaço de nomes, como em C.

### 2.2 Funções

| Função | O que faz |
|---|---|
| `inicializar_tabela()` | Cria o escopo global. Deve ser chamada uma vez, antes do `yyparse` |
| `entrar_escopo()` | Empilha um novo escopo vazio |
| `sair_escopo()` | Desempilha o escopo atual e libera todos os seus símbolos |
| `nivel_escopo()` | Quantos escopos estão abertos (1 = só o global) |
| `restaurar_escopo(nivel)` | Fecha escopos até restar `nivel` abertos. Ressincroniza a tabela depois de um erro sintático (`NIVEL_GLOBAL` = 1) |
| `inserir_variavel(nome, tipo)` | Insere no escopo atual. Retorna `1` em caso de sucesso e `0` se o nome já existe **no mesmo escopo** (ou se a tabela não foi inicializada) |
| `inserir_funcao(nome, tipo, n, tipos)` | Insere uma função no escopo atual, guardando a aridade e o tipo de cada parâmetro. Retorna `0` se o nome já existe no escopo |
| `buscar_variavel(nome)` | Procura do escopo atual para fora, até o global. Retorna o `Simbolo*` ou `NULL` (serve também para funções) |

### 2.3 Regras de escopo

- Um nome só pode ser declarado uma vez por escopo.
- Um escopo interno pode **redeclarar** um nome de um escopo externo (sombreamento). Por exemplo, `int x; { int x; }` é válido.
- Cada bloco `{ ... }` cria um escopo novo. `if` e `while` sem chaves não criam escopo.
- O `for` abre um escopo próprio: a variável de `for (int i = 0; ...)` só existe dentro do laço.
- O corpo de uma função não abre escopo além do dos parâmetros: parâmetro e variável local de mesmo nome na mesma função são uma redeclaração.
- Variáveis e comandos no nível do programa ficam no escopo global.

### 2.4 Integração com o parser

Em `parser.y`:

- **`main`** chama `inicializar_tabela()` antes de `yyparse()`.
- **`bloco_inicio: LBRACE`** chama `entrar_escopo()` e **`bloco_fim: RBRACE`** chama `sair_escopo()`. **`for_inicio: FOR`** abre o escopo do `for`, fechado ao final da regra do laço.
- **`iniciar_funcao`**, chamada logo depois do `)` do cabeçalho, registra a função com `inserir_funcao`, abre o escopo da função e insere os parâmetros nele. O escopo é fechado ao final da regra `definicao_funcao`.
- **`tipo`** (`INT`, `FLOAT`, `CHAR`) guarda o tipo na variável global `tipo_atual`. Assim, numa declaração múltipla como `int a, b;`, todos os nomes da lista recebem o mesmo tipo.
- **`declaracao_variavel`** (`ID` ou `ID ASSIGN expressao`) chama `inserir_variavel` com `tipo_atual`. Toda declaração passa por essa regra, via `declaracao → tipo lista_declaracoes SEMI`.
- Depois de um erro sintático, o Bison pode descartar um `}` e deixar escopos abertos. Por isso `restaurar_escopo(NIVEL_GLOBAL)` é chamado ao começar cada função e antes das verificações finais (`decisoes-tecnicas.md`, D12).

Se a inserção de uma variável falhar, o parser imprime em `stderr`:

```
Erro Semantico na linha N: Variavel 'x' ja declarada neste escopo.
```

O parser **continua** depois desse erro, para reportar os demais. Como há erro, o programa não é executado e o interpretador sai com código 1.

### 2.5 Análise semântica

Além da tabela de símbolos, o parser faz as verificações abaixo, todas nas ações das regras do Bison (uma única passada, [decisoes-tecnicas.md](decisoes-tecnicas.md), D6). Duas funções de `ast.c` dão suporte às checagens de tipo:

- **`tipo_da_expressao(no)`**: infere o tipo (`INT`, `FLOAT`, `CHAR` ou `TIPO_STRING`) de uma expressão, percorrendo a AST recursivamente e consultando a tabela de símbolos para identificadores e chamadas. Operadores relacionais e lógicos resultam em `INT`; operadores aritméticos resultam em `FLOAT` se algum operando for `FLOAT`, senão `INT`. Retorna `-1` quando o tipo não pode ser determinado (por exemplo, identificador não declarado). Nesse caso o erro de "não declarada" já foi reportado em outro ponto, e o chamador ignora o `-1` para não gerar um segundo erro.
- **`tipos_compativeis(esperado, obtido)`**: decide se um valor do tipo `obtido` pode ser usado onde se espera `esperado`, seguindo a Seção 6.1 da Especificação: `int` e `float` convertem entre si (`float → int` trunca) e `char` nunca converte implicitamente.

As verificações:

1. **Tipo de retorno** (`RETURN expressao SEMI`): compara o tipo da expressão com `tipo_retorno_atual`, variável global atualizada em `iniciar_funcao`. Também detecta `return` fora de qualquer função.
2. **Atribuição e inicialização**: tanto `expressao ASSIGN expressao` quanto `declaracao_variavel: ID ASSIGN expressao` comparam o tipo do valor com o tipo do alvo.
3. **Operadores**: `criar_binaria_verificada` e `criar_unaria_verificada` rejeitam operando do tipo `TIPO_STRING` (strings só podem ser argumento de `printf`, nunca parte de uma conta).
4. **Chamada de função** (`validar_chamada`): confere que o símbolo é uma função (`eh_funcao`), que o número de argumentos bate com `num_params` e que cada argumento é compatível com o parâmetro correspondente (`checar_assinatura`). Os tipos dos argumentos são calculados no escopo da chamada. O nó `NO_CHAMADA` é sempre construído, mesmo com erro, para o parser continuar com segurança.
5. **Chamadas pendentes**: se a função ainda não foi vista (definida mais abaixo, ou recursão mútua), a chamada vai para uma lista e é conferida no fim do parse (`verificar_chamadas_pendentes`), quando todas as funções já estão registradas. Se ainda assim não existir, o erro é "função não declarada".
6. **`scanf`** (`validar_scanf`): o formato deve ser `"%d"`, `"%f"` ou `"%c"`, a variável deve existir e seu tipo deve ser **exatamente** o do especificador (sem passar por `tipos_compativeis`).
7. **`return` ausente**: ao reduzir `definicao_funcao`, `lista_sempre_retorna` analisa os caminhos do corpo (Especificação, Seção 8.2). Um comando "sempre retorna" se for um `return`, um bloco com um comando que sempre retorna, ou um `if` **com `else`** em que os dois ramos sempre retornam. Laços não contam. A checagem é pulada se a função teve erro sintático.
8. **`main`**: a assinatura deve ser `int main()` (verificada em `iniciar_funcao`) e a função deve existir (`verificar_main`, depois do parse).
9. **Variáveis**: uso de variável não declarada, inclusive em `++`/`--` (`criar_incremento`).

Todos os erros semânticos usam a macro `ERRO_SEMANTICO`, que incrementa `total_erros` e imprime em `stderr`. As linhas apontadas vêm de `%locations` (`@n.first_line`), isto é, a linha do próprio símbolo com problema. Com `total_erros > 0` o programa não é executado.

---

## 3. Limitações atuais

- A AST e o escopo global não são liberados ao final da execução.
- O tipo de retorno de uma chamada a função ainda não declarada (definida mais abaixo) não é conferido no ponto de uso, e os erros dessas chamadas saem no fim da lista.
- `printf` não valida os especificadores contra os tipos dos argumentos.
- Não há `void`: toda função precisa declarar um tipo de retorno `int`, `float` ou `char`.
- `++` e `--` aplicados a uma variável `char` não são rejeitados: a conta é feita como `int` e convertida de volta para `char`.
- O parser aceita alvos de atribuição que não são variáveis (`5 = x`); o erro só aparece na execução.

## 4. Como testar

```bash
make clean && make
printf 'int main() { int x = 2 + 3 * 4; if (x > 5) { x = 1; } else x = 0; return 0; }\n' | ./build/compilador -ast   # imprime a árvore
printf 'int main() { int x; int x; return 0; }\n' | ./build/compilador -ast                                          # erro semântico (redeclaração)
printf 'int main() { int x; { int x; } return 0; }\n' | ./build/compilador -ast                                      # válido (sombreamento)
printf 'int f() { return 3.14; } int main() { return 0; }\n' | ./build/compilador -ast                                 # ok, float->int trunca (Seção 6.1)
printf "int f() { return 'a'; } int main() { return 0; }\n" | ./build/compilador -ast                                  # erro semântico (char->int não permitido)
printf 'int soma(int a, int b) { return a + b; } int main() { int x = soma(1); return 0; }\n' | ./build/compilador   # erro semântico (aridade)
printf 'int f() { } int main() { return 0; }\n' | ./build/compilador                                                  # erro semântico (função pode terminar sem return)
```

Em todos os casos com erro, o programa não é executado e o interpretador sai com código 1. A suíte completa roda com `make test` ([decisoes-tecnicas.md](decisoes-tecnicas.md), D15).