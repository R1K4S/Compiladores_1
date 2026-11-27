# Especificação da Linguagem MiniC (V1.1)

> **V1.1 — alinhada à implementação.** A V1 proibia algumas construções que o interpretador já aceita. Em vez de remover recursos funcionando, a V1.1 os oficializa: comandos e variáveis no nível do programa (globais), `++`/`--`, atribuição encadeada, sombreamento de variáveis e `char` em expressões aritméticas. Também passa a valer que programas com erro **não são executados**. Cada mudança está marcada no texto com **(V1.1)**.

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
| Operadores | `+ - * / % == != < > <= >= && || ! = ++ --` |
| Pontuação | `; , ( ) { }` |
| Comentários | `// ...` e `/* ... */` (descartados na análise léxica) |

MiniC é, estruturalmente, uma sequência de **definições de função**, **declarações de variável** e **comandos**, sendo obrigatória a presença de `int main()` como ponto de entrada.

> **Nível do programa (V1.1):** fora das funções podem aparecer declarações de variável e comandos. Eles são executados **em ordem, antes de `main`**. As variáveis declaradas ali são **globais**: ficam no escopo global e são visíveis em todas as funções definidas depois delas. Diferente de C, comandos soltos (como `printf(...)`) também são aceitos nesse nível.

```c
int limite = 3;                 // variável global

int dentro(int x) {
    return x <= limite;         // funções enxergam a global
}

int main() {
    printf("%d\n", dentro(2));  // 1
    return 0;
}
```

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

Várias variáveis do mesmo tipo podem ser declaradas de uma vez: `int a, b = 1, c;`.

```ebnf
declaracao_variavel ::= tipo item_declaracao { "," item_declaracao } ";"
item_declaracao     ::= IDENTIFICADOR [ "=" expressao ]
tipo                ::= "int" | "float" | "char"
```

Uma variável declarada sem inicialização começa com valor `0` (ou `0.0`, ou o caractere de código 0).

### 3.2 Escopo

O MiniC usa **escopo léxico (estático)**: a visibilidade de uma variável é determinada pelo bloco `{ ... }` textual onde ela foi declarada, e não pelo caminho de execução em tempo real.

Regras:
- Um bloco interno **enxerga** as variáveis dos blocos que o envolvem (escopo pai), desde que estejam visíveis naquele ponto.
- Uma variável declarada em um bloco **deixa de existir** assim que esse bloco termina (`}`).
- Um nome **não pode** ser declarado duas vezes no **mesmo** escopo (erro semântico).
- **(V1.1)** Um bloco interno **pode** declarar uma variável com o mesmo nome de uma variável de um bloco externo (sombreamento, ou *shadowing*), como em C. Dentro do bloco interno, o nome se refere à variável interna; ao sair dele, a externa volta a ser visível com o valor que tinha.

```c
int x = 1;
{
    int x = 10;        // válido: sombreia o x de fora
    printf("%d", x);   // 10
}
printf("%d", x);       // 1
```

- Corpos de função não abrem um escopo extra: um parâmetro e uma variável local de mesmo nome na mesma função são uma redeclaração (erro).
- A variável declarada na inicialização de um `for` existe apenas dentro do laço (Seção 7.3).

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
| Incremento / decremento **(V1.1)** | `++  --` | tipo da variável |

### 5.2 Aritméticos

`+`, `-`, `*` e `/` operam sobre `int` e `float` (com conversão automática — ver Seção 6). O operador `%` (resto da divisão inteira) é permitido **somente** entre dois valores `int`; com `float` gera erro de execução. Divisão ou resto por zero também geram erro de execução.

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

Apenas a atribuição simples `=` é suportada (sem `+=`, `-=` etc.). **(V1.1)** Como em C, a atribuição é uma **expressão**: seu valor é o valor gravado na variável (já convertido para o tipo dela). Por isso ela pode ser encadeada, com associatividade à direita, e usada dentro de outras expressões.

```c
int x = 10;
x = 20;              // válido

int a, b;
a = b = 10;          // válido (V1.1): b recebe 10, depois a recebe 10

int d;
int e = (d = 4) + 1; // d = 4, e = 5
```

O lado esquerdo de `=` precisa ser uma variável. Algo como `5 = x;` é reconhecido pelo parser, mas é rejeitado com erro de execução.

### 5.6 Incremento e decremento (V1.1)

`x++`, `++x`, `x--` e `--x` são abreviações de `x = x + 1` e `x = x - 1`. Valem para variáveis `int` e `float`.

**Atenção, diferença em relação ao C:** no MiniC as quatro formas devolvem o valor **já atualizado**. Em `int y = x++;`, `y` recebe o valor novo de `x`, e não o antigo. Usadas como comando isolado (`i++;`) ou no passo de um `for`, as quatro formas têm o mesmo efeito que em C.

```c
int i = 5;
i++;                 // i = 6
for (i = 0; i < 3; i++) { printf("%d ", i); }   // 0 1 2
```

### 5.7 Precedência e associatividade

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

`++` e `--` só se aplicam diretamente a um identificador (`i++`, `--i`), então não entram nesta tabela.

### 5.8 Fora de escopo

Os seguintes operadores **não existem** no MiniC e seu uso gera erro léxico ou sintático: `+=`, `-=`, `*=`, `/=`, `?:` (operador ternário).

## 6. Conversão de Tipos

O MiniC define conversões implícitas **apenas** entre `int` e `float`. Um valor `char` nunca é convertido implicitamente em atribuição, inicialização, argumento ou `return`, mas **(V1.1)** pode participar de expressões aritméticas (Seção 6.2).

### 6.1 Conversão em atribuição/inicialização

| De | Para | Regra | Exemplo |
|---|---|---|---|
| `int` | `float` | Conversão exata (nenhuma perda) | `int x = 10; float y = x;` → `y = 10.0` |
| `float` | `int` | Trunca a parte fracionária (não arredonda) | `float x = 10.8; int y = x;` → `y = 10` |
| `char` | `int`/`float` | **Não permitido** | `int x = 'A';` → erro semântico |
| `int`/`float` | `char` | **Não permitido** | `char c = 10;` → erro semântico |

### 6.2 Promoção de tipos dentro de uma expressão aritmética

Esta regra resolve o tipo resultante de `+`, `-`, `*`, `/` e `%` **antes** de qualquer conversão de atribuição ser aplicada:

- Se **ambos os operandos são `int`**, o resultado é `int`. Em particular, `/` entre dois `int` é **divisão inteira** (trunca o resultado), independentemente do tipo da variável que vai receber o valor.
- Se **pelo menos um operando é `float`**, o outro é promovido para `float` e o resultado da operação é `float`.
- **(V1.1)** Um operando `char` entra na conta com o seu código (por exemplo, `'A'` vale 65) e é tratado como `int`. Assim, `char + int` e `char + char` resultam em `int`, e `char + float` resulta em `float`.
- O menos unário preserva o tipo do operando.

```c
char c = 'A';
int codigo = c + 0;   // válido: 65
int prox = c + 1;     // válido: 66
int x = c;            // ERRO semântico: char não converte para int (Seção 6.1)
char d = c + 1;       // ERRO semântico: o resultado é int, e int não converte para char
```

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

**(V1.1)** O corpo de `if`, `else`, `while` e `for` é um **comando qualquer**: um bloco `{ ... }` ou um único comando sem chaves, como em C (`if (x > 0) x = 0;`). Usar chaves continua sendo o recomendado. Um `else` pertence sempre ao `if` mais próximo que ainda não tem `else`.

**Limitação atual:** um bloco precisa ter **pelo menos um comando**. `{ }` vazio, inclusive como corpo de função, é erro sintático.

### 7.1 `if` e `if-else`

```ebnf
comando_if ::= "if" "(" expressao ")" comando [ "else" comando ]
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
comando_while ::= "while" "(" expressao ")" comando
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
comando_for ::= "for" "(" ( declaracao_variavel | expressao ";" ) expressao ";" expressao ")" comando
```

**Os três componentes do `for` são obrigatórios**: não existe `for (;;)`. Uma variável declarada na inicialização (`for (int i = 0; ...)`) só existe dentro do laço.

```c
for (int i = 0; i < 10; i = i + 1) {
    printf("Iteracao: %d\n", i);
}
```

### 7.4 Fora de escopo

`switch`, `do-while`, `break` e `continue` não existem no MiniC. Um laço só pode terminar naturalmente pela condição se tornar falsa.

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

Uma função pode retornar `int`, `float` ou `char` (não existe `void`). Ela **deve** executar um `return` com uma expressão compatível com esse tipo; `return;` sem expressão não é permitido. O valor retornado é convertido para o tipo de retorno declarado (por exemplo, `return 3.9;` numa função `int` devolve `3`).

**Limitação atual:** a ausência de `return` ainda não é detectada (Seção 11, item 7). Uma função que termina sem executar `return` devolve `0` do seu tipo.

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

**A função precisa estar definida antes (acima) do ponto onde é chamada.** A análise semântica é feita numa única passada durante o parse, então chamar uma função definida mais abaixo no arquivo é erro semântico (`funcao '...' nao declarada`). Como não há protótipos, isso também impede a recursão mútua (Seção 8.4).

### 8.4 Recursão

**Recursão direta é permitida**: uma função pode chamar a si mesma. Recursão **mútua** (`a` chama `b`, que chama `a`) não é possível, porque uma das duas teria de ser chamada antes de ser definida (Seção 8.3). O interpretador limita a profundidade a **1.000 chamadas aninhadas**; acima disso, gera erro de execução. Isso não exige nenhuma regra semântica adicional além das já definidas — decorre naturalmente do modelo de execução: cada chamada de função cria um novo ambiente de execução (um novo conjunto de variáveis locais e parâmetros) empilhado sobre o ambiente do chamador (ver Seção 8.1). Enquanto essa pilha de ambientes existir, chamadas recursivas funcionam sem tratamento especial.

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

`main` não recebe parâmetros e precisa retornar `int`; outra assinatura é erro semântico. É o ponto de entrada: o interpretador executa primeiro os comandos do nível do programa (Seção 2), na ordem em que aparecem, e depois chama `main`. Todas as demais funções são alcançadas a partir dela. O valor retornado por `main` vira o **código de saída** do interpretador.

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

Também são reconhecidos `%%` (imprime `%`) e as sequências `\n` (nova linha) e `\t` (tabulação) na string de formato. Não há largura nem precisão: `%.2f` e `%5d` geram erro de execução. O tipo do argumento **não é verificado** contra o especificador. O valor é convertido: `%d` com um `float` trunca, e `%f` com um `int` imprime com casas decimais. Argumentos a mais ou a menos que os especificadores geram erro de execução.

### 9.2 Entrada — `scanf`

Diferente do C, o `scanf` do MiniC recebe a variável **diretamente**, sem o operador `&` (que não existe na linguagem, já que não há ponteiros):

```c
int idade;

printf("Digite sua idade: ");
scanf("%d", idade);
```

Cada `scanf` lê **um** valor, e o formato deve ser exatamente `"%d"`, `"%f"` ou `"%c"`. O tipo da variável deve ser **exatamente** o do especificador (`%d` → `int`, `%f` → `float`, `%c` → `char`), sem conversão; o contrário é erro semântico. `%c` ignora espaços e quebras de linha antes do caractere. **`%s` não é suportado para entrada** (não haveria onde guardar a string lida, já que não existem variáveis do tipo string nem arrays de `char`).

## 10. Strings Literais

Strings existem **apenas como literais de texto**, usados exclusivamente como argumento de `printf`:

```c
printf("Olá, mundo!\n");
```

Não é possível declarar uma variável do tipo string, nem armazenar uma string em uma variável — isso exigiria arrays de `char` (ou ponteiros), ambos fora de escopo.

## 11. Regras Semânticas e Catálogo de Erros

O interpretador detecta e rejeita, **antes da execução**, qualquer programa que viole as regras abaixo. O mesmo vale para erros léxicos (caractere inválido) e sintáticos. Todos os erros encontrados são listados em `stderr`, seguidos de `N erro(s) encontrado(s). O programa nao foi executado.`, e o interpretador termina com código de saída `1`. Nenhum comando do programa é executado.

Erros que só podem ser detectados durante a execução (divisão ou resto por zero, `%` com `float`, recursão acima do limite, formato inválido no `printf`, falha de leitura no `scanf`) interrompem o programa com `Erro de execucao: ...` e código de saída `1`.

Este catálogo serve de referência para os testes do analisador semântico.

| # | Regra violada | Exemplo | Resultado esperado | Status |
|---|---|---|---|---|
| 1 | Uso de variável não declarada | `x = 10;` (sem declarar `x` antes) | Erro semântico | ✅ Implementado |
| 2 | Redeclaração no mesmo escopo | `int x; int x;` | Erro semântico | ✅ Implementado |
| 3 | Uso de variável fora do escopo em que foi declarada | Ver exemplo da Seção 3.2 (`y` usado após o `if`) | Erro semântico | ✅ Implementado (via pilha de escopos) |
| 4 | Número incorreto de argumentos em chamada de função | `soma(int a,int b){...}` chamada como `soma(10);` | Erro semântico | ✅ Implementado |
| 5 | Tipo incompatível em atribuição/inicialização | `char c = 10.5;` | Erro semântico | ✅ Implementado |
| 6 | Ausência da função `int main()` | Programa sem `main` | Erro semântico | ✅ Implementado |
| 7 | `return` ausente em função com tipo de retorno, ou com tipo incompatível | Função `int` que não executa `return`, ou que retorna um `char` | Erro semântico | ⚠️ Parcial: tipo incompatível é verificado; ausência de `return` em algum caminho ainda não |
| 8 | Argumento de tipo incompatível | `letra(10)` com parâmetro `char` | Erro semântico | ✅ Implementado |
| 9 | Chamada de algo que não é função, ou de função não declarada (ou declarada abaixo) | `int n; n(1);` | Erro semântico | ✅ Implementado |
| 10 | Função ou parâmetro redeclarado | `int f(int a, int a)` | Erro semântico | ✅ Implementado |
| 11 | `main` com assinatura diferente de `int main()` | `float main(int x)` | Erro semântico | ✅ Implementado |
| 12 | `return` fora de função | `return 0;` no nível do programa | Erro semântico | ✅ Implementado |
| 13 | `scanf` com formato inválido ou tipo diferente do especificador | `scanf("%d", f)` com `f` float | Erro semântico | ✅ Implementado |
| 14 | String literal em operação | `1 + "a"`, `!"a"` | Erro semântico | ✅ Implementado |

## 12. Recursos Fora de Escopo

Os itens abaixo **não existem** no MiniC. Seu uso deve ser rejeitado pelo analisador léxico, sintático ou semântico, conforme o caso — nunca silenciosamente ignorado.

| Categoria | Recursos excluídos |
|---|---|
| Memória e tipos compostos | ponteiros, aritmética de ponteiros, arrays, `struct`, `union`, `enum`, `typedef`, `malloc`, `free` |
| Pré-processador | macros, `#include`, `#define`, diretivas em geral |
| Controle de fluxo | `switch`, `do-while`, `break`, `continue`, operador ternário `?:` |
| Operadores | atribuições compostas (`+=`, `-=`, `*=`, `/=`) |
| Dados | strings armazenadas em variáveis |

Esses itens compõem o **backlog de expansão** do projeto (fora desta especificação) e só devem ser considerados depois que o núcleo aqui descrito estiver implementado e validado por completo.

## 13. Gramática Formal Consolidada (EBNF)

Esta seção reúne, em um único lugar, todas as regras gramaticais apresentadas ao longo do documento, já com as mudanças da V1.1.

```ebnf
programa             ::= item { item }
item                 ::= definicao_funcao
                       | comando                      (* V1.1: declaracoes e comandos globais *)

definicao_funcao     ::= tipo IDENTIFICADOR "(" [ lista_parametros ] ")" "{" lista_comandos "}"
lista_parametros     ::= parametro { "," parametro }
parametro            ::= tipo IDENTIFICADOR

lista_comandos       ::= comando { comando }          (* pelo menos 1 comando *)
bloco                ::= "{" lista_comandos "}"

declaracao_variavel  ::= tipo item_declaracao { "," item_declaracao } ";"
item_declaracao      ::= IDENTIFICADOR [ "=" expressao ]
tipo                 ::= "int" | "float" | "char"

comando              ::= declaracao_variavel
                       | expressao ";"
                       | comando_if
                       | comando_while
                       | comando_for
                       | comando_return
                       | comando_printf
                       | comando_scanf
                       | bloco

comando_if           ::= "if" "(" expressao ")" comando [ "else" comando ]
comando_while        ::= "while" "(" expressao ")" comando
comando_for          ::= "for" "(" ( declaracao_variavel | expressao ";" )
                             expressao ";" expressao ")" comando
comando_return       ::= "return" expressao ";"
comando_printf       ::= "printf" "(" STRING_LITERAL [ "," lista_argumentos ] ")" ";"
comando_scanf        ::= "scanf" "(" STRING_LITERAL "," IDENTIFICADOR ")" ";"

expressao            ::= atribuicao
atribuicao           ::= IDENTIFICADOR "=" atribuicao  (* V1.1: encadeavel, assoc. a direita *)
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
                       | STRING_LITERAL               (* so como argumento de printf, para %s *)
                       | IDENTIFICADOR
                       | IDENTIFICADOR ( "++" | "--" ) (* V1.1 *)
                       | ( "++" | "--" ) IDENTIFICADOR (* V1.1 *)
                       | IDENTIFICADOR "(" [ lista_argumentos ] ")"
                       | "(" expressao ")"
lista_argumentos     ::= expressao { "," expressao }
```

A gramática acima descreve a linguagem aceita. O arquivo `parser.y` a implementa de forma mais compacta: uma única regra `expressao` com declarações `%left`/`%right` (Seção 5.7). Por isso o parser também aceita alvos de atribuição que não são identificadores (`5 = x`), que são rejeitados na execução (Seção 5.5). A correspondência entre esta EBNF e as regras do Bison está em [`gramatica_minic.md`](gramatica_minic.md).

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
