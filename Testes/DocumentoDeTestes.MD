# Como fizemos

De início planejamos que cada membro testasse no mínimo o que implementou. Como todos participaram do projeto e acabaram modificando, nem que seja um pouco, implementações de colegas para atender aos requisitos, cada um está livre para testar, além do mínimo, todas as funcionalidades que desejar verificar.

## Como rodar

### Automático (recomendado)

```bash
make test
```

Roda todos os `Testes/*/*.c` e os programas de referência `programas_teste/*.c`, e compara cada saída (stdout, stderr e código de saída) com o `.esperado` ao lado. Espaços no fim das linhas são ignorados. Se existir um `.in` com o mesmo nome, ele vira a entrada do `scanf` (ex.: `Gustavo/07_scanf.in`). Os testes rodam com a opção `-ast`, então a AST também é comparada. O comando termina com `Resultado: 33 passaram, 0 falharam.`

Para adicionar um teste: crie o `.c` (e o `.in`, se precisar), rode `make test-atualizar`, confira o `.esperado` gerado com `git diff` e commite os dois.

### Manual

```bash
make clean && make
./build/compilador Testes/Felipe/01_declaracoes.c        # só a saída do programa
./build/compilador -ast Testes/Felipe/12_arvore.c        # também a AST e as variáveis globais
echo "42 3.14 A" | ./build/compilador Testes/Gustavo/07_scanf.c
```

O arquivo vai como argumento, e assim o stdin fica livre para o `scanf`. Cada pasta de `Testes/` pertence a um membro. As mensagens de erro saem pelo `stderr`. Se houver qualquer erro léxico, sintático ou semântico, o programa não é executado e o código de saída é `1`.

## Programas de referência (`programas_teste/`)

Fatorial, Fibonacci e MDC eram os programas de referência em C da Sprint 1. Eles foram reescritos em MiniC válido e entram no `make test`, com a entrada do `scanf` no `.in` de cada um.

| Programa | Entrada (`.in`) | Saída esperada | Diferenças em relação à versão em C |
|---|---|---|---|
| `fatorial.c` | `10` | `Fatorial de 10 = 3628800` | `int` no lugar de `unsigned long long` (maior fatorial exato: 12!), `scanf` sem `&` |
| `fibonacci.c` | `30` | `832040` | Cálculo iterativo no lugar da memoização com vetor (o MiniC não tem vetores nem `long`); maior termo exato: `fibonacci(46)` |
| `mdc.c` | `48 18` | `Mdc (48 , 18) -> funcao mdc basica = 6` | Um `scanf` por variável, sem `&`; `while` no lugar do `for` com corpo vazio |

Os executáveis gerados com `gcc` a partir das versões em C (`fatorial`, `fibo`, `mdc`) foram removidos do repositório e estão no `.gitignore`.

## Felipe

**Escopo testado:** declaração de variáveis, escopo, expressões aritméticas, operadores relacionais e lógicos, atribuição e conversão de tipos, erros semânticos (variáveis, tipos e funções) e erros de execução.

Os testes 01 a 05 imprimem cada resultado seguido do valor esperado, então o teste passa quando os dois valores são iguais na mesma linha. Nos testes 06 a 11 o resultado esperado é uma mensagem de erro.

### Comportamento correto

| Teste | Arquivo | O que verifica | Resultado |
|---|---|---|---|
| 01 | `01_declaracoes.c` | `int`, `float` e `char` com e sem valor inicial; várias variáveis na mesma declaração | Passou. Valores iguais aos esperados (5, 2.5, A, 0, 3) |
| 02 | `02_escopo.c` | Bloco enxerga variável de fora; variável do bloco "esconde" a de fora (shadowing); alteração dentro do bloco persiste; três níveis de bloco | Passou |
| 03 | `03_aritmetica.c` | `+ - * / %`, precedência, parênteses, menos unário, operações misturando `int` e `float` | Passou. Ex.: `7 / 2.0 = 3.5` |
| 04 | `04_relacionais_e_logicos.c` | `< > <= >= == !=`, `&&`, `\|\|`, `!`, precedência de `&&` sobre `\|\|` e curto-circuito | Passou. O lado direito não foi executado quando o resultado já estava decidido |
| 05 | `05_atribuicao_e_conversao.c` | Atribuição simples, encadeada, valor de uma atribuição, conversão `float` para `int` (trunca) e `int` para `float` | Passou. Ex.: `int x = 10.7` guarda `10` |

### Erros semânticos (detectados antes de executar)

| Teste | Arquivo | Erros provocados | Mensagens obtidas |
|---|---|---|---|
| 06 | `06_erros_de_variavel.c` | Variável não declarada; variável redeclarada no mesmo escopo; variável de um bloco usada fora dele | `Variavel 'x' nao declarada` (linha 12), `Variavel 'b' nao declarada` (linha 14), `Variavel 'y' nao declarada` (linha 19) e `Variavel 'z' ja declarada neste escopo` (linha 22) |
| 07 | `07_erros_de_tipo.c` | Inicialização incompatível; atribuição incompatível; operador aritmético com string | Linhas 3, 6 e 8, uma mensagem para cada caso |
| 08 | `08_erros_de_funcao.c` | Argumentos a menos; tipo de argumento errado; função inexistente; chamar algo que não é função; função redeclarada; parâmetro duplicado; `return` de tipo errado; `return` fora de função; `main` com assinatura errada | Linhas 6, 7, 10, 12, 13, 14, 16, 18 e 8, uma mensagem para cada caso. O erro da linha 8 (função inexistente) sai por último, porque chamadas a funções ainda não vistas só são conferidas no fim da análise |
| 09 | `09_erro_sem_main.c` | Programa sem `main` | `funcao 'main' nao encontrada`. O programa não é executado |

### Erros de execução

| Teste | Arquivo | Situação | Mensagem obtida |
|---|---|---|---|
| 10 | `10_erro_divisao_por_zero.c` | Divisão por zero depois de imprimir "antes" | `Erro de execucao: divisao por zero.` A execução para na divisão |
| 11 | `11_erro_resto_com_float.c` | `%` com operando `float` | `Erro de execucao: o operador % exige operandos inteiros.` |

### Outros

| Teste | Arquivo | O que verifica | Resultado |
|---|---|---|---|
| 12 | `12_arvore.c` | Construção e impressão da AST | Passou. Com `-ast`, a árvore é impressa antes da execução |
| 13 | `13_funcoes_pendentes.c` | Definição e chamada de funções, `return` e recursão | Passou. `dobro de 4: 8`, `fatorial de 5: 120`, passagem por valor `v=1 r=99` |

## Allan

**Escopo testado:** _a preencher pelo Allan._

## Gustavo

**Escopo testado:** estruturas de controle (`if-else` aninhado, `while`, `for`), `printf` com todos os especificadores suportados, incremento/decremento (`++`/`--`), `scanf`, combinações de estruturas, escopo próprio do `for`, e erros semânticos relacionados a `scanf` e ao uso de strings em expressões.

Esses testes **complementam** os do Felipe: enquanto ele cobre declarações, escopo simples, aritmética, relacionais/lógicos e erros de variáveis/tipos/funções, aqui o foco é nas **estruturas de execução** e nas **primitivas de E/S**.

### Comportamento correto

| Teste | Arquivo | O que verifica | Resultado esperado |
|---|---|---|---|
| 01 | `01_if_else_aninhado.c` | Cadeia de `else-if` (somente o ramo correto executa), `if-else` aninhado no ramo `then`, condição falsa direciona para `else` | `resultado esperado: B  obtido: 75` / `x < y e x == 5: esperado 1  obtido: 1` / `a e zero, else executou: esperado 1  obtido: 1` |
| 02 | `02_while.c` | `while` simples (conta 0–4), `while` que nunca executa, `while` aninhado (tabela 2×3), acúmulo de soma com `while` | Contagem de 0 a 4; "while zero vezes ok"; tabela com `lin=` e `col=`; `soma 1..10 esperado: 55  obtido: 55` |
| 03 | `03_for.c` | `for` com declaração no `init`, `for` com expressão no `init`, `for` que não executa, `for` aninhado (tabuada 2×3), valor acumulado fora do `for` | `soma 0..4 esperado: 10  obtido: 10`; sequência `j=1 j=2 j=3`; "for zero iteracoes ok"; tabuada do 2 e do 3; `ultimo valor de vazou esperado: 2  obtido: 2` |
| 04 | `04_printf.c` | `%d`, `%f`, `%c`, `%s`, múltiplos especificadores na mesma chamada, `%%` literal, string sem especificadores, expressão como argumento, negativo | Cada linha imprime o valor esperado; `%%` aparece como `%`; `2 + 3 = 5`; `-7` correto |
| 05 | `05_incremento_decremento.c` | `x++`, `++x`, `x--`, `--x` como comandos isolados, `i++` no `for`, `j--` no `for` (contagem regressiva), `++` em `float` | Sequência `6 7 6 5`; `soma 1..5 com i++: esperado 15  obtido: 15`; `5! com j--: esperado 120  obtido: 120`; `2.5 apos f++: esperado 3.500000  obtido: 3.500000` |
| 06 | `06_estruturas_combinadas.c` | `if` dentro de `for` (filtra pares), `while` com `if` para parar na condição, `if-else` dentro de `while` para classificar, escopo de variável declarada dentro de `if` | Pares `0 2 4 6 8`; `primeiro n com n*n > 20: esperado 5  obtido: 5`; sequência "impar/par" de 1 a 5; `total (3*10 + 4*10): esperado 70  obtido: 70` |
| 07 | `07_scanf.c` | Leitura de `int` com `%d`, `float` com `%f`, `char` com `%c`; os valores lidos são impressos com `printf` | Com entrada `42 3.14 A` (arquivo `07_scanf.in`): `inteiro lido: 42` / `float lido: 3.140000` / `char lido: A` |
| 11 | `11_programa_completo.c` | Programa que usa `if-else`, `for`, `while`, `++`, operações `int`/`float` mistas e `printf` com `%d` e `%f` — exercita todas as construções implementadas juntas | `Aprovado! Media: 8.000000`; repeticoes 0–2; Contador 0–1; `5 + 2.0 = 7.000000`; `5/2 int=2  5/2 float=2.000000  5.0/2=2.500000` |

### Erros semânticos (detectados antes de executar)

| Teste | Arquivo | Erros provocados | Mensagens esperadas |
|---|---|---|---|
| 08 | `08_erros_scanf.c` | Formato `%s` inválido para `scanf`; `%d` em variável `float`; `%f` em variável `int`; variável não declarada no `scanf` | Linha 6: `scanf aceita apenas os formatos "%d", "%f" ou "%c"`; Linha 9: incompatibilidade de tipo com `%d`; Linha 12: incompatibilidade de tipo com `%f`; Linha 15: `Variavel 'fantasma' nao declarada` |
| 09 | `09_erros_printf_string.c` | Operador `+` entre `int` e string literal; `==` entre duas strings; `!` unário sobre string | Linha 5: `operador '+' nao pode ser aplicado a uma string`; Linha 8: `operador '==' nao pode ser aplicado a uma string`; Linha 11: `operador '!' nao pode ser aplicado a uma string` |
| 10 | `10_escopo_for.c` | Variável `i` declarada no `init` do `for` usada fora do laço | Linha 8: `Variavel 'i' nao declarada` |

### Notas técnicas

**Testes 03 e 10 — `for` com declaração no `init`.** O `for` do MiniC (`parser.y`) tem escopo próprio: a regra `for_inicio` chama `entrar_escopo()` antes do `LPAREN` e `sair_escopo()` é chamado depois do corpo do laço. Por isso `int i = 0` declarado no `for_init` é visível dentro do corpo, mas desaparece após o `}` do laço. O teste 10 verifica exatamente esse comportamento como erro semântico.

**Testes 04 e 11 — `printf` processa escapes no interpretador.** O `\n` e o `\t` dentro de strings de formato são processados pelo `case NO_PRINTF:` do `interpretador.c` (laço sobre a string com `if (*f == '\\')`). Isso não depende do lexer. Já os escapes em literais `char` (`'\n'`) são traduzidos pelo lexer (teste `Ricardo/01_escapes_char.c`).

**Teste 05 — `++` e `--` são açúcar sintático.** A `criar_incremento` no `parser.y` transforma `i++` e `++i` em `i = i + 1` durante o parse, e o interpretador não precisa de nenhum `case` extra. O valor de `i++` já é o incrementado (diferente do C), pois o nó gerado é um `NO_ATRIBUICAO` que escreve e devolve o novo valor.

**Teste 07 — `scanf` lê do stdin.** O programa é passado como argumento e a entrada do `scanf` vem do stdin: `echo "42 3.14 A" | ./build/compilador Testes/Gustavo/07_scanf.c`. No `make test`, a entrada vem de `07_scanf.in`.

## Ricardo

**Escopo testado:** recuperação de erros sintáticos, linha exata nas mensagens de erro (`%locations`), escapes em literais `char`, blocos vazios, chamada de função antes da sua definição, recursão mútua e detecção de `return` ausente.

Esses testes cobrem as correções e recursos acrescentados depois dos testes de Felipe e Gustavo.

### Comportamento correto

| Arquivo | O que verifica | Resultado |
|---|---|---|
| `01_escapes_char.c` | Escapes em literais `char`: `'\n'` `'\t'` `'\0'` `'\\'` `'\''` | Passou. `codigos: 10 9 0 92 39` |
| `02_blocos_vazios.c` | `{ }` como bloco e como corpo de `if`/`else`/`while`/`for` | Passou. `i depois do for vazio: 3`, `funcao vazia devolve: 0` |
| `03_chamada_antes_da_definicao.c` | `main` chama funções definidas abaixo dela; recursão mútua `par`/`impar` | Passou. `dobro(21) = 42`, `par(10) = 1, par(7) = 0`, `impar(7) = 1` |

### Erros (detectados antes de executar)

| Arquivo | Erros provocados | Mensagens obtidas |
|---|---|---|
| `01_recuperacao_de_erros.c` | Três erros sintáticos e um semântico no mesmo programa: o parser continua depois de `;` ou `}` e reporta todos numa execução | Erros sintáticos nas linhas 6, 10 e 13 e semântico na linha 15; `4 erro(s) encontrado(s). O programa nao foi executado.`, código de saída 1 |
| `02_linha_exata.c` | Erros semânticos em expressões que continuam na linha seguinte | Erros nas linhas 7 (`y`), 9 (`c`) e 12 (`+` com string), a linha do próprio símbolo; `3 erro(s) encontrado(s)`, código de saída 1 |
| `04_erros_de_funcao_tardia_e_return.c` | `return` ausente (`if` sem `else`, `return` só no `while`) e erros de chamadas conferidas no fim (aridade, função inexistente) | Erros nas linhas 14, 20, 9 e 10, nessa ordem (os de chamada saem por último); `4 erro(s) encontrado(s)`, código de saída 1 |
