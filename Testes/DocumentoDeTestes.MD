# Como fizemos

De início planejamos que cada membro testasse no mínimo o que implementou. Como todos participaram do projeto e acabaram modificando, nem que seja um pouco, implementações de colegas para atender aos requisitos, cada um está livre para testar, além do mínimo, todas as funcionalidades que desejar verificar.

## Como rodar

A partir da raiz do projeto:

```bash
make clean && make
for f in Testes/Felipe/*.c; do echo "=== $f"; ./build/compilador < $f 2>&1; done
```

Para rodar um único teste:

```bash
./build/compilador < Testes/Felipe/01_declaracoes.c
```

Cada pasta de `Testes/` pertence a um membro. Os programas são escritos em MiniC e entram pela entrada padrão. As mensagens de erro saem pelo `stderr`, então aparecem antes do restante da saída quando há pipe.

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
| 06 | `06_erros_de_variavel.c` | Variável não declarada; variável redeclarada no mesmo escopo; variável de um bloco usada fora dele | `Variavel 'x' nao declarada` (linhas 12, 14 e 19) e `Variavel 'z' ja declarada neste escopo` (linha 22) |
| 07 | `07_erros_de_tipo.c` | Inicialização incompatível; atribuição incompatível; operador aritmético com string | Linhas 3, 6 e 8, uma mensagem para cada caso |
| 08 | `08_erros_de_funcao.c` | Argumentos a menos; tipo de argumento errado; função inexistente; chamar algo que não é função; função redeclarada; parâmetro duplicado; `return` de tipo errado; `return` fora de função; `main` com assinatura errada | Linhas 6, 7, 8, 10, 12, 13, 14, 16 e 18, uma mensagem para cada caso |
| 09 | `09_erro_sem_main.c` | Programa sem `main` | `funcao 'main' nao encontrada` (aviso: os comandos globais ainda executam) |

### Erros de execução

| Teste | Arquivo | Situação | Mensagem obtida |
|---|---|---|---|
| 10 | `10_erro_divisao_por_zero.c` | Divisão por zero depois de imprimir "antes" | `Erro de execucao: divisao por zero.` A execução para na divisão |
| 11 | `11_erro_resto_com_float.c` | `%` com operando `float` | `Erro de execucao: o operador % exige operandos inteiros.` |

### Outros

| Teste | Arquivo | O que verifica | Resultado |
|---|---|---|---|
| 12 | `12_arvore.c` | Construção e impressão da AST | Passou. A árvore é impressa antes da execução |
| 13 | `13_funcoes_pendentes.c` | Definição e chamada de funções, `return` e recursão | **Pendente (#51).** Hoje não imprime nada porque o interpretador ainda não executa funções. Reexecutar quando o #51 estiver completo |

## Allan

## Gustavo

## Ricardo