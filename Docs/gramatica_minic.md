# Gramática Livre de Contexto - MiniC (EBNF)

Este documento descreve a primeira versão da gramática formal da nossa linguagem (MiniC) utilizando a notação EBNF (Extended Backus-Naur Form). Como estabelecido, é um subconjunto da linguagem C, removendo elementos complexos como ponteiros, `structs` e laços `for`.

> **Nota sobre a notação:**
> * `( ... )` agrupa elementos.
> * `[ ... ]` ou `?` indica que o elemento é opcional (0 ou 1 vez).
> * `{ ... }` ou `*` indica repetição (0 ou mais vezes).
> * `|` indica alternativas.
> * Terminais (palavras-chave e símbolos) estão entre aspas `" "`.

## 1. Estrutura Global
O programa é composto por uma sequência de declarações de variáveis globais e/ou definições de funções.

```ebnf
Program ::= { Declaration | FunctionDef }

Type ::= "int" | "float" | "char"
```

## 2. Funções
```ebnf
FunctionDef ::= Type Identifier "(" [ ParameterList ] ")" Block

ParameterList ::= Parameter { "," Parameter }

Parameter ::= Type Identifier
```

## 3. Comandos e Blocos (Statements)
```ebnf
Block ::= "{" { Statement } "}"

Statement ::= Declaration
            | ExpressionStatement
            | IfStatement
            | WhileStatement
            | ReturnStatement
            | Block

Declaration ::= Type Identifier [ "=" Expression ] ";"

ExpressionStatement ::= [ Expression ] ";"

IfStatement ::= "if" "(" Expression ")" Statement [ "else" Statement ]

WhileStatement ::= "while" "(" Expression ")" Statement

ReturnStatement ::= "return" [ Expression ] ";"
```

## 4. Expressões (Expressions)
As expressões estão ordenadas da menor para a maior precedência, garantindo a avaliação correta das operações matemáticas e lógicas.

```ebnf
Expression ::= Assignment

Assignment ::= Identifier "=" Assignment 
             | LogicalOr

LogicalOr ::= LogicalAnd { "||" LogicalAnd }

LogicalAnd ::= Equality { "&&" Equality }

Equality ::= Relational { ("==" | "!=") Relational }

Relational ::= Additive { ("<" | "<=" | ">" | ">=") Additive }

Additive ::= Multiplicative { ("+" | "-") Multiplicative }

Multiplicative ::= Unary { ("*" | "/") Unary }

Unary ::= ("!" | "-") Unary 
        | Primary

Primary ::= Identifier
          | IntegerLiteral
          | FloatLiteral
          | CharLiteral
          | "(" Expression ")"
          | FunctionCall

FunctionCall ::= Identifier "(" [ ArgumentList ] ")"

ArgumentList ::= Expression { "," Expression }
```

## 5. Notas Léxicas (Lexer)
Os itens abaixo são tratados na fase de análise léxica e geralmente omitidos da árvore sintática:
* **Identifier**: Começa com letra ou `_`, seguido por letras, números ou `_`.
* **Literais**: 
  * `IntegerLiteral`: Sequência de dígitos numéricos.
  * `FloatLiteral`: Dígitos separados por um `.` (ponto).
  * `CharLiteral`: Um caractere entre aspas simples (ex: `'a'`).
* **Comentários**: Ignorados pelo Parser.
  * Linha: `// ...`
  * Bloco: `/* ... */`
