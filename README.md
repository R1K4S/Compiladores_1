# Compiladores_1 — Interpretador MiniC

Interpretador de **MiniC**, um subconjunto da linguagem C, desenvolvido com **Flex** e **Bison** como projeto da disciplina **Compiladores 1 (FGA0003 — UnB)**.

O interpretador lê um programa MiniC, faz a análise léxica, sintática e semântica, monta a árvore sintática abstrata (AST) e executa o programa percorrendo a AST.

- Especificação completa da linguagem: [Docs/Especificacao.md](Docs/Especificacao.md)
- Gramática (EBNF): [Docs/gramatica_minic.md](Docs/gramatica_minic.md)
- AST e tabela de símbolos: [Docs/AST_e_Tabela_de_Simbolos.md](Docs/AST_e_Tabela_de_Simbolos.md)
- Instalação do ambiente (Linux, WSL, macOS): [Docs/Guia de Instalação.md](Docs/Guia%20de%20Instala%C3%A7%C3%A3o.md)
- Testes: [Testes/DocumentoDeTestes.MD](Testes/DocumentoDeTestes.MD)
- Como contribuir: [Docs/Contribuicao.md](Docs/Contribuicao.md)

## Dependências

| Ferramenta | Uso |
|---|---|
| `gcc` | Compilar o código C |
| `make` | Automatizar a build |
| `flex` | Gerar o analisador léxico (`lexer/lexel.l`) |
| `bison` | Gerar o analisador sintático (`parser/parser.y`) |

``````````bash
# Ubuntu / Debian / WSL
sudo apt update && sudo apt install -y build-essential flex bison

# macOS
brew install flex bison
``````````

## Compilar

``````````bash
make clean && make
``````````

O executável é gerado em `build/compilador`.

## Rodar

``````````bash
./build/compilador programa.c
``````````

Passe o arquivo como argumento: assim a entrada padrão fica livre para o `scanf` do programa.

``````````bash
echo "5" | ./build/compilador fatorial.c       # programa que usa scanf
./build/compilador < programa.c                # também aceito, mas o scanf não terá entrada
``````````

### O que é impresso

1. `Analise sintatica concluida com sucesso!`
2. A AST do programa, indentada
3. A saída do programa MiniC (`printf`)
4. As variáveis globais ao final da execução

Erros vão para `stderr`. Se houver **qualquer** erro léxico, sintático ou semântico, o programa **não é executado** e o interpretador termina com código de saída `1`. Sem erros, o código de saída é o valor retornado por `main`.

| Situação | Mensagem (exemplo) | Código de saída |
|---|---|---|
| Erro léxico | `Caractere invalido: @` | 1 |
| Erro sintático | `Erro sintático na linha 3: syntax error, unexpected SEMI` | 1 |
| Erro semântico | `Erro Semantico na linha 8: Variavel 'i' nao declarada.` | 1 |
| Erro de execução | `Erro de execucao: divisao por zero.` | 1 |
| Sucesso | — | valor de `return` em `main` |

## Exemplo

Arquivo `fatorial.c`:

``````````c
int fatorial(int n) {
    if (n <= 1) {
        return 1;
    }
    return n * fatorial(n - 1);
}

int main() {
    int n;
    printf("Digite um numero: ");
    scanf("%d", n);
    printf("Fatorial de %d = %d\n", n, fatorial(n));
    return 0;
}
``````````

``````````bash
$ echo "5" | ./build/compilador fatorial.c 2>&1 | grep -v '^  '
Analise sintatica concluida com sucesso!
Programa
Digite um numero: Fatorial de 5 = 120

Variaveis globais ao final da execucao:
``````````

(O `grep -v '^  '` esconde as linhas da AST.)

Repare que o `scanf` do MiniC recebe a variável **sem** `&`, pois a linguagem não tem ponteiros.

## O que a linguagem suporta

- Tipos `int`, `float` e `char`; declaração com ou sem inicialização, inclusive múltipla (`int a, b = 1;`)
- Operadores `+ - * / %`, `== != < > <= >=`, `&& || !`, menos unário, `++`/`--`, atribuição `=`
- `if`/`else`, `while`, `for`
- Funções com parâmetros, `return`, recursão; `int main()` é obrigatória
- `printf` (`%d %f %c %s %%`, `\n`, `\t`) e `scanf` (`%d %f %c`)
- Comentários `//` e `/* */`

Fora do escopo: vetores, ponteiros, `struct`, strings em variáveis, `switch`, `do-while`, `break`, `continue`, `+=`, `void`, `#include`. Detalhes na [Especificação](Docs/Especificacao.md).

## Rodar os testes

``````````bash
for f in Testes/*/*.c; do
  echo "=== $f"
  ./build/compilador "$f" < /dev/null 2>&1 | grep -v '^  '
done
``````````

O teste com `scanf` precisa de entrada:

``````````bash
echo "42 3.14 A" | ./build/compilador Testes/Gustavo/07_scanf.c
``````````

## Estrutura

``````````text
lexer/lexel.l          Analisador léxico (Flex)
parser/parser.y        Gramática, construção da AST, análise semântica e main()
src/ast.c, ast.h       Nós da AST, impressão e inferência de tipos
src/tabela.c, .h       Tabela de símbolos com pilha de escopos
src/ambiente.c, .h     Valores e escopos em tempo de execução
src/interpretador.c,.h Execução da AST
Docs/                  Especificação, gramática e guias
Testes/                Programas de teste por integrante
``````````

## Licença

Distribuído sob a licença MIT. Veja [LICENSE](LICENSE).
