# Especificação da Linguagem MiniC (V1)

> Este documento define, de forma completa e autocontida, a sintaxe e a semântica da linguagem **MiniC** — um subconjunto de C projetado para ser interpretado. Ele foi escrito para ser lido por qualquer pessoa com conhecimento de compiladores, mesmo sem contato prévio com o projeto: todo termo específico do MiniC é definido antes de ser usado, e cada construção vem acompanhada de exemplo, gramática e, quando relevante, do erro que ela deve produzir se usada incorretamente.

## 1. Introdução

**MiniC** é um subconjunto da linguagem C, projetado para servir de linguagem-alvo de um **interpretador construído com Flex + Bison**. Em vez de implementar C por completo o MiniC seleciona um núcleo de comandos, suficiente para escrever programas imperativos reais: variáveis, expressões, condicionais, laços, funções e E/S básica.

Este documento cobre a linguagem na íntegra: tipos, escopo, operadores, conversões, comentários, estruturas de controle, laços, funções, entrada/saída, regras semânticas e uma gramática formal consolidada.

## 2. Visão Geral da Linguagem

Antes de entrar em cada construção em detalhe, um resumo do que compõe um programa MiniC:

| Categoria léxica | Exemplos |
|---|---|
| Palavras-chave | `int`, `float`, `char`, `if`, `else`, `while`, `for`, `return` |
| Identificadores | `x`, `contador`, `calcularMedia` |
| Literais | `10`, `9.5`, `'A'`, `"Olá"` |
| Operadores | `+ - * / % == != < > <= >= && || ! =` |
| Pontuação | `; , ( ) { }` |
| Comentários | `// ...` e `/* ... */` (descartados na análise léxica) |

MiniC é, estruturalmente, uma sequência de **definições de função**, sendo obrigatória a presença de `int main()` como ponto de entrada. Não existem instruções soltas no nível de programa fora de uma função assim como em C.

> **Sem variáveis globais na V1:** a gramática do MiniC (Seção 13) só permite `definicao_funcao` no nível de programa — não existe uma regra para declarar variável fora de uma função. Ou seja, **toda variável do MiniC vive dentro de alguma função ou bloco**; não há estado compartilhado global. Essa restrição elimina, de saída, a necessidade de um escopo global especial na tabela de símbolos.

```c
// Estrutura mínima de um programa MiniC válido
int main() {
    return 0;
}
```

## 3. Tipos de Dados e Variáveis

O MiniC possui exatamente **três tipos primitivos**, e nenhum tipo composto (sem arrays, sem `struct`):

| Tipo | Representa | Exemplo de literal |
|---|---|---|
| `int` | Números inteiros | `10`, `-3`, `0` |
| `float` | Números de ponto flutuante | `9.5`, `-0.25` |
| `char` | Um único caractere | `'A'`, `'z'`, `'\n'` |

### 3.1 Declaração de variáveis

Toda variável **deve ser declarada antes de ser utilizada** — não existe inferência de tipo nem declaração implícita (diferente de linguagens como Python). A inicialização no momento da declaração é opcional:

```c
int contador;          // declarada, sem valor inicial definido
int numero = 10;       // declarada e inicializada
float media = 8.5;
char letra = 'A';
```

**Consequência para o interpretador:** o analisador semântico deve inserir cada variável na tabela de símbolos do escopo corrente no momento da declaração, e qualquer uso anterior a essa declaração (ou em um escopo onde ela não é visível) é um erro semântico — ver Seção 11.

**Gramática (EBNF) desta construção:**

```ebnf
declaracao_variavel ::= tipo IDENTIFICADOR [ "=" expressao ] ";"
tipo                ::= "int" | "float" | "char"
```

### 3.2 Escopo

O MiniC usa **escopo léxico (estático)**: a visibilidade de uma variável é determinada pelo bloco `{ ... }` textual onde ela foi declarada, e não pelo caminho de execução em tempo real.

Regras:
- Um bloco interno **enxerga** as variáveis dos blocos que o envolvem (escopo pai), desde que estejam visíveis naquele ponto.
- Uma variável declarada em um bloco **deixa de existir** assim que esse bloco termina (`}`).
- Na V1, **não é permitido** redeclarar um nome já usado em um escopo aninhado dentro do mesmo escopo — ou seja, sombreamento (*shadowing*) de variáveis não é suportado.

```c
int main() {
    int x = 10;

    if (x > 0) {
        int y = 20;         // y só existe dentro deste bloco if
        printf("%d", y);
    }

    // Uso de y aqui seria erro semântico: "y" saiu de escopo.

    return 0;
}
```

**Implicação de implementação:** a tabela de símbolos deve ser organizada como uma pilha de escopos encadeados — ao entrar em um bloco `{`, cria-se um novo escopo cujo pai é o escopo que o envolve; ao sair do bloco `}`, o escopo é descartado.

## 4. Comentários

O MiniC segue exatamente a convenção de comentários do C. Comentários são **completamente descartados durante a análise léxica** — eles nunca chegam ao analisador sintático.

```c
// comentário de uma linha
int x = 10;

/*
   comentário
   de múltiplas linhas
*/
int y = 20;
```

## 5. Operadores

### 5.1 Categorias de operadores

| Categoria | Operadores | Tipo do resultado |
|---|---|---|
| Aritméticos | `+  -  *  /  %` | `int` ou `float` (depende dos operandos) |
| Relacionais | `==  !=  <  >  <=  >=` | sempre `int` (`0` ou `1`) |
| Lógicos | `&&  \|\|  !` | sempre `int` (`0` ou `1`) |
| Atribuição | `=` | tipo da variável atribuída |

### 5.2 Aritméticos

`+`, `-`, `*` e `/` operam sobre `int` e `float` (com conversão automática — ver Seção 6). O operador `%` (resto da divisão inteira) é permitido **somente** entre dois valores `int`.

```c
int a = 10;
int b = 3;

int soma = a + b;    // 13
int resto = a % b;   // 1
```

### 5.3 Relacionais

Produzem sempre um valor `int`: `1` para verdadeiro, `0` para falso.

```c
int maior_de_idade = idade >= 18;   // 1 ou 0
```

### 5.4 Lógicos

Também produzem `int` (`0`/`1`) e usam **avaliação de curto-circuito** (*short-circuit*): em `a && b`, se `a` for falso, `b` não é avaliado; em `a || b`, se `a` for verdadeiro, `b` não é avaliado.

```c
int aprovado = (nota >= 6.0) && (faltas <= 10);
```

### 5.5 Atribuição

Apenas a atribuição simples `=` é suportada, e **a atribuição não é encadeável**: `a = b = 10;`, válido em C, **não é permitido** no MiniC V1 (é um erro sintático). Cada atribuição é tratada como uma unidade independente — decisão que simplifica a gramática e evita ambiguidade sobre se atribuição é "expressão" (que pode ser aninhada em qualquer lugar) ou apenas um tipo de comando.

```c
int x = 10;
x = 20;      // válido

int a, b;
a = b = 10;  // NÃO permitido na V1 — erro sintático
```

### 5.6 Precedência e associatividade

A tabela abaixo resume a precedência (da mais alta para a mais baixa) e é o que deve orientar diretamente as declarações `%left`/`%right` no arquivo `parser.y`:

| Precedência | Operadores | Associatividade |
|---|---|---|
| 1 (mais alta) | `!` (lógico não), `-` (unário) | direita |
| 2 | `*  /  %` | esquerda |
| 3 | `+  -` | esquerda |
| 4 | `<  >  <=  >=` | esquerda |
| 5 | `==  !=` | esquerda |
| 6 | `&&` | esquerda |
| 7 | `\|\|` | esquerda |
| 8 (mais baixa) | `=` | direita |

### 5.7 Fora de escopo na V1

Os seguintes operadores **não existem** no MiniC V1 e seu uso deve gerar erro léxico ou sintático: `+=`, `-=`, `*=`, `/=`, `++`, `--`, `?:` (operador ternário).

## 6. Conversão de Tipos

O MiniC define conversões **apenas** entre `int` e `float`; não há conversão implícita envolvendo `char`.

### 6.1 Conversão em atribuição/inicialização

| De | Para | Regra | Exemplo |
|---|---|---|---|
| `int` | `float` | Conversão exata (nenhuma perda) | `int x = 10; float y = x;` → `y = 10.0` |
| `float` | `int` | Trunca a parte fracionária (não arredonda) | `float x = 10.8; int y = x;` → `y = 10` |
| `char` | `int`/`float` | **Não permitido** na V1 | `int x = 'A';` → erro semântico |
| `int`/`float` | `char` | **Não permitido** na V1 | `char c = 10;` → erro semântico |

### 6.2 Promoção de tipos dentro de uma expressão aritmética

Esta regra resolve o tipo resultante de `+`, `-`, `*` e `/` **antes** de qualquer conversão de atribuição ser aplicada:

- Se **ambos os operandos são `int`**, o resultado é `int`. Em particular, `/` entre dois `int` é **divisão inteira** (trunca o resultado), independentemente do tipo da variável que vai receber o valor.
- Se **pelo menos um operando é `float`**, o outro é promovido para `float` e o resultado da operação é `float`.
- `char` nunca participa de expressão aritmética diretamente (ver Seção 6.1) — logo não entra nesta regra.

```c
int a = 5;
float b = 2.0;

float c = a + b;   // a é promovido a float -> c = 7.0

int x = 5 / 2;      // divisão inteira -> x = 2
float y = 5 / 2;    // 5/2 é calculado como int (=2) e SÓ DEPOIS convertido -> y = 2.0, e não 2.5!
float z = 5.0 / 2;  // aqui já é divisão float/float -> z = 2.5
```

O último par de exemplos (`y` e `z`) é intencional: reforça que a regra de promoção olha para os **tipos dos operandos na expressão**, não para o tipo da variável de destino.

## 7. Estruturas de Controle

### 7.1 `if` e `if-else`

```ebnf
comando_if ::= "if" "(" expressao ")" bloco [ "else" ( bloco | comando_if ) ]
```

```c
if (nota >= 9.0) {
    printf("Excelente!");
} else if (nota >= 6.0) {
    printf("Aprovado!");
} else {
    printf("Reprovado!");
}
```

Note que **não existe um `else if` como palavra reservada** — o que parece um `else if` é, na verdade, um `else` cujo corpo é outro comando `if`. Isso é o mesmo truque usado pela gramática oficial de C e simplifica a gramática do MiniC (uma regra a menos).

A condição pode ser qualquer expressão cujo valor seja tratável como booleano: `0` é falso, qualquer valor diferente de `0` é verdadeiro (igual ao C).

### 7.2 Laço `while`

```ebnf
comando_while ::= "while" "(" expressao ")" bloco
```

```c
int i = 0;

while (i < 5) {
    printf("Valor de i: %d\n", i);
    i = i + 1;
}
```

### 7.3 Laço `for`

```ebnf
comando_for ::= "for" "(" declaracao_ou_expressao ";" expressao ";" expressao ")" bloco
```

Na V1, **os três componentes do `for` são obrigatórios** — não existe `for (;;)`.

```c
for (int i = 0; i < 10; i = i + 1) {
    printf("Iteracao: %d\n", i);
}
```

### 7.4 Fora de escopo na V1

`switch`, `do-while`, `break` e `continue` não existem no MiniC V1. Um laço só pode terminar naturalmente pela condição se tornar falsa.

## 8. Funções

### 8.1 Definição

```ebnf
definicao_funcao ::= tipo IDENTIFICADOR "(" [ lista_parametros ] ")" bloco
lista_parametros  ::= parametro { "," parametro }
parametro         ::= tipo IDENTIFICADOR
```

```c
int somar(int x, int y) {
    int resultado = x + y;
    return resultado;
}
```

Parâmetros são passados **por valor**: a função recebe uma cópia, e alterá-los dentro da função não afeta a variável do chamador (não há ponteiros no MiniC, então passagem por referência simplesmente não existe).

### 8.2 Tipos de retorno e `return`

Uma função pode retornar `int`, `float` ou `char`. Se a função declara um tipo de retorno, ela **deve** executar um `return` com uma expressão compatível com esse tipo — `return;` sem expressão não é permitido na V1.

```c
float calcularMedia(float a, float b) {
    return (a + b) / 2.0;
}
```

### 8.3 Chamada de função

```c
int total = somar(5, 7);
```

O número de argumentos na chamada deve ser **exatamente igual** ao número de parâmetros na definição, e cada argumento deve ter tipo compatível com o parâmetro correspondente (mesmas regras de conversão da Seção 6).

### 8.4 Recursão

**Recursão é permitida** no MiniC V1: uma função pode chamar a si mesma, direta ou indiretamente. Isso não exige nenhuma regra semântica adicional além das já definidas — decorre naturalmente do modelo de execução: cada chamada de função cria um novo ambiente de execução (um novo conjunto de variáveis locais e parâmetros) empilhado sobre o ambiente do chamador (ver Seção 8.1). Enquanto essa pilha de ambientes existir, chamadas recursivas funcionam sem tratamento especial.

```c
int fatorial(int n) {
    if (n <= 1) {
        return 1;
    } else {
        return n * fatorial(n - 1);
    }
}
```

### 8.5 Função `main`

Todo programa MiniC deve conter obrigatoriamente:

```c
int main() {
    ...
}
```

`main` é o ponto de entrada da execução — é a primeira (e, na prática, geralmente a única) função chamada diretamente pelo interpretador, e todas as demais funções do programa são alcançadas a partir dela.

## 9. Entrada e Saída

`printf` e `scanf` são **primitivas da linguagem MiniC**, executadas diretamente pelo interpretador — não são chamadas à biblioteca padrão de C (`<stdio.h>`), embora tenham sintaxe e propósito inspirados nas funções homônimas de C. Por serem primitivas da própria linguagem, elas não exigem `#include` e não seguem exatamente as mesmas regras das funções C reais (isso é o que permite, por exemplo, o `scanf` do MiniC dispensar o operador `&` — ver 9.2).

### 9.1 Saída — `printf`

| Especificador | Tipo esperado |
|---|---|
| `%d` | `int` |
| `%f` | `float` |
| `%c` | `char` |
| `%s` | string literal |

```c
int idade = 20;
float nota = 9.5;
char inicial = 'A';

printf("%d\n", idade);
printf("%f\n", nota);
printf("%c\n", inicial);
printf("%s\n", "Olá");
```

Strings só existem como **literais** usados diretamente em uma chamada de `printf` — não há tipo string nem variáveis do tipo string no MiniC (Seção 10).

### 9.2 Entrada — `scanf`

Diferente do C, o `scanf` do MiniC recebe a variável **diretamente**, sem o operador `&` (que não existe na linguagem, já que não há ponteiros):

```c
int idade;

printf("Digite sua idade: ");
scanf("%d", idade);
```

Especificadores suportados na entrada: `%d`, `%f`, `%c`. **`%s` não é suportado para entrada** na V1 (não haveria onde guardar a string lida, já que não existem variáveis do tipo string nem arrays de `char`).

## 10. Strings Literais

Strings existem **apenas como literais de texto**, usados exclusivamente como argumento de `printf`:

```c
printf("Olá, mundo!\n");
```

Não é possível declarar uma variável do tipo string, nem armazenar uma string em uma variável — isso exigiria arrays de `char` (ou ponteiros), ambos fora de escopo na V1.

## 11. Regras Semânticas e Catálogo de Erros

O interpretador deve detectar e rejeitar, antes da execução, qualquer programa que viole as regras abaixo. Este catálogo serve diretamente de referência para os testes/validações manuais do analisador semântico.

| # | Regra violada | Exemplo | Resultado esperado |
|---|---|---|---|
| 1 | Uso de variável não declarada | `x = 10;` (sem declarar `x` antes) | Erro semântico |
| 2 | Redeclaração no mesmo escopo | `int x; int x;` | Erro semântico |
| 3 | Uso de variável fora do escopo em que foi declarada | Ver exemplo da Seção 3.2 (`y` usado após o `if`) | Erro semântico |
| 4 | Número incorreto de argumentos em chamada de função | `soma(int a,int b){...}` chamada como `soma(10);` | Erro semântico |
| 5 | Tipo incompatível em atribuição/inicialização | `char c = 10.5;` | Erro semântico |
| 6 | Ausência da função `int main()` | Programa sem `main` | Erro semântico |
| 7 | `return` ausente em função com tipo de retorno, ou com tipo incompatível | Função `int` que não executa `return`, ou que retorna um `float` incompatível | Erro semântico |

## 12. Recursos Fora de Escopo (V1)

Os itens abaixo **não existem** no MiniC V1. Seu uso deve ser rejeitado pelo analisador léxico, sintático ou semântico, conforme o caso — nunca silenciosamente ignorado.

| Categoria | Recursos excluídos |
|---|---|
| Memória e tipos compostos | ponteiros, aritmética de ponteiros, arrays, `struct`, `union`, `enum`, `typedef`, `malloc`, `free` |
| Pré-processador | macros, `#include`, `#define`, diretivas em geral |
| Controle de fluxo | `switch`, `do-while`, `break`, `continue`, operador ternário `?:` |
| Operadores | `++`, `--`, atribuições compostas (`+=`, `-=`, `*=`, `/=`) |
| Dados | strings armazenadas em variáveis |

Esses itens compõem o **backlog de expansão** do projeto (fora desta especificação V1) e só devem ser considerados depois que o núcleo aqui descrito estiver implementado e validado por completo.

## 13. Gramática Formal Consolidada (EBNF)

Esta seção reúne, em um único lugar, todas as regras gramaticais apresentadas ao longo do documento — útil como referência direta na hora de escrever o `parser.y`.

```ebnf
programa            ::= { definicao_funcao }

definicao_funcao     ::= tipo IDENTIFICADOR "(" [ lista_parametros ] ")" bloco
lista_parametros     ::= parametro { "," parametro }
parametro            ::= tipo IDENTIFICADOR

bloco                ::= "{" { declaracao | comando } "}"

declaracao_variavel  ::= tipo IDENTIFICADOR [ "=" expressao ] ";"
tipo                 ::= "int" | "float" | "char"

comando              ::= declaracao_variavel
                        | comando_expressao
                        | comando_if
                        | comando_while
                        | comando_for
                        | comando_return
                        | comando_printf
                        | comando_scanf
                        | bloco

comando_expressao    ::= expressao ";"
comando_if           ::= "if" "(" expressao ")" bloco [ "else" ( bloco | comando_if ) ]
comando_while        ::= "while" "(" expressao ")" bloco
comando_for          ::= "for" "(" ( declaracao_variavel | comando_expressao ) expressao ";" expressao ")" bloco
comando_return       ::= "return" expressao ";"
comando_printf       ::= "printf" "(" STRING_LITERAL { "," expressao } ")" ";"
comando_scanf        ::= "scanf" "(" STRING_LITERAL "," IDENTIFICADOR ")" ";"

expressao            ::= atribuicao
atribuicao           ::= IDENTIFICADOR "=" expressao_logica_or
                        | expressao_logica_or
expressao_logica_or  ::= expressao_logica_and { "||" expressao_logica_and }
expressao_logica_and ::= expressao_igualdade { "&&" expressao_igualdade }
expressao_igualdade  ::= expressao_relacional { ( "==" | "!=" ) expressao_relacional }
expressao_relacional ::= expressao_aditiva { ( "<" | ">" | "<=" | ">=" ) expressao_aditiva }
expressao_aditiva    ::= expressao_multiplicativa { ( "+" | "-" ) expressao_multiplicativa }
expressao_multiplicativa
                     ::= expressao_unaria { ( "*" | "/" | "%" ) expressao_unaria }
expressao_unaria     ::= ( "!" | "-" ) expressao_unaria
                        | expressao_primaria
expressao_primaria   ::= NUMERO_INT
                        | NUMERO_FLOAT
                        | CHAR_LITERAL
                        | IDENTIFICADOR
                        | IDENTIFICADOR "(" [ lista_argumentos ] ")"
                        | "(" expressao ")"
lista_argumentos     ::= expressao { "," expressao }
```

## 14. Exemplo Completo

O programa abaixo usa praticamente todas as construções descritas neste documento: tipos, operadores, `if-else`, `for`, `while`, função com parâmetro e retorno, e E/S.

```c
float calcularMedia(int a, int b) {
    return (a + b) / 2.0;
}

int main() {
    int nota1 = 7;
    int nota2 = 9;
    float media;
    int i;

    media = calcularMedia(nota1, nota2);

    if (media >= 6.0) {
        printf("Aprovado! Media: %f\n", media);
    } else {
        printf("Reprovado! Media: %f\n", media);
    }

    for (i = 0; i < 3; i = i + 1) {
        printf("Repeticao numero %d\n", i);
    }

    int contador = 0;
    while (contador < 2) {
        printf("Contador: %d\n", contador);
        contador = contador + 1;
    }

    return 0;
}
```

Saída esperada:

```text
Aprovado! Media: 8.000000
Repeticao numero 0
Repeticao numero 1
Repeticao numero 2
Contador: 0
Contador: 1
```

Como o programa acima não usa `scanf`, ele continua sendo sintaticamente válido em C real — bastando acrescentar `#include <stdio.h>` no topo do arquivo antes de compilar com `gcc` — o que permite conferir a saída esperada antes mesmo de o interpretador estar pronto, útil já na Sprint 1, quando os programas de referência estão sendo escritos. Programas que usam `scanf` (Seção 9.2) não devem ser comparados dessa forma, já que o `scanf` do MiniC diverge intencionalmente do `scanf` real do C (Seção 1).

## 15. Objetivo da Versão 1

A V1 do MiniC prioriza uma implementação completa e correta de:

1. análise léxica;
2. análise sintática;
3. análise semântica (tabela de símbolos, escopos, tipos);
4. avaliação de expressões;
5. execução de estruturas de controle (`if`, `while`, `for`);
6. execução de funções (definição, chamada, parâmetros, `return`);
7. entrada e saída básica (`printf`, `scanf`).

Recursos mais avançados de C (Seção 12) ficam deliberadamente fora do escopo desta versão, para manter a linguagem pequena, previsível e adequada à implementação de um interpretador didático dentro do tempo disponível da equipe.
