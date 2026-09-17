# Especificação da Linguagem

Este documento define os elementos básicos da nossa linguagem, que segue uma sintaxe baseada em C, porém com simplificações estruturais (por exemplo, não implementaremos laços `for`, ponteiros ou `structs`).

## 1. Tipos de Dados
A linguagem suporta os seguintes tipos primitivos:
* **`int`**: Números inteiros.
* **`float`**: Números de ponto flutuante.
* **`char`**: Caracteres simples.

## 2. Declaração de Variáveis
A declaração exige o tipo explícito e permite a inicialização no momento da declaração.
```c
int x = 20;
float pi = 3.14;
char letra = 'a';
```

## 3. Estruturas de Controle
As estruturas de fluxo de execução suportadas são:
* **Condicionais**: `if` e `else`
* **Laços de repetição**: `while`
* **Retorno de função**: `return`

## 4. Operadores e Expressões
A linguagem avaliará expressões usando os seguintes operadores:
* **Aritméticos**: Operações matemáticas (ex: `+`, `-`, `*`, `/`).
* **Relacionais**: Comparações (ex: `==`, `!=`, `>`, `<`, `>=`, `<=`).
* **Lógicos**: Operações booleanas (ex: `&&`, `||`, `!`).

## 5. Delimitadores e Pontuação
Os símbolos utilizados para estruturação do código são:
* **`(` e `)`**: Agrupamento de expressões, condições de `if/while` e parâmetros.
* **`,`**: Separação de elementos.
* **`;`**: Finalizador de instruções obrigatório.

## 6. Comentários
Suporte a comentários para documentação e anotações no código-fonte:
* **Linha única**: `// comentário de linha`
* **Múltiplas linhas**: `/* comentário de bloco */`
