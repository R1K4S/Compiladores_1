# Gramática do MiniC (V1.1)

Este documento descreve a gramática **exatamente como implementada** em `parser/parser.y` e os tokens produzidos por `lexer/lexel.l`. A versão em EBNF, organizada por nível de precedência e mais fácil de ler, está na [Especificação, Seção 13](Especificacao.md#13-gramática-formal-consolidada-ebnf). As duas descrevem a mesma linguagem.

O Bison gera esta gramática **sem conflitos** shift/reduce ou reduce/reduce.

## 1. Tokens (lexer/lexel.l)

| Token | Padrão / lexema | Valor (`yylval`) |
|---|---|---|
| `INT` `FLOAT` `CHAR` | `int` `float` `char` | — |
| `IF` `ELSE` `WHILE` `FOR` `RETURN` | `if` `else` `while` `for` `return` | — |
| `PRINTF` `SCANF` | `printf` `scanf` | — |
| `ID` | `[a-zA-Z_][a-zA-Z0-9_]*` | `str_val` (nome) |
| `NUM` | `[0-9]+` | `int_val` |
| `NUM_FLOAT` | `[0-9]+\.[0-9]+` | `float_val` |
| `CHAR_LITERAL` | `'x'` ou `'\x'` | `int_val` (código do caractere) |
| `STRING_LITERAL` | `"..."` (sem quebra de linha) | `str_val` (sem as aspas) |
| `PLUS` `MINUS` `TIMES` `DIVIDE` `MOD` | `+ - * / %` | — |
| `INC` `DEC` | `++ --` | — |
| `EQ` `NEQ` `LT` `GT` `LE` `GE` | `== != < > <= >=` | — |
| `AND` `OR` `NOT` | `&& \|\| !` | — |
| `ASSIGN` | `=` | — |
| `LPAREN` `RPAREN` `LBRACE` `RBRACE` `SEMI` `COMMA` | `( ) { } ; ,` | — |

São descartados: espaços, tabulações e quebras de linha, comentários `// ...` e `/* ... */`. Qualquer outro caractere gera `Caractere invalido: X` e conta como erro: o programa não é executado.

Palavras-chave têm prioridade sobre `ID` (regras declaradas antes). Literais `float` exigem dígitos dos dois lados do ponto: `1.` e `.5` não são aceitos.

## 2. Regras (parser/parser.y)

``````````yacc
programa         : lista_itens ;

lista_itens      : lista_itens item
                 | item ;

item             : comando
                 | definicao_funcao ;

definicao_funcao : tipo ID LPAREN parametros_opt RPAREN LBRACE lista_comandos RBRACE ;

parametros_opt   : /* vazio */
                 | lista_parametros ;
lista_parametros : parametro
                 | lista_parametros COMMA parametro ;
parametro        : tipo ID ;

lista_comandos   : lista_comandos comando
                 | comando ;                      /* pelo menos 1 comando */

comando          : IF LPAREN expressao RPAREN comando          %prec LOWER_THAN_ELSE
                 | IF LPAREN expressao RPAREN comando ELSE comando
                 | WHILE LPAREN expressao RPAREN comando
                 | FOR LPAREN for_init expressao SEMI expressao RPAREN comando
                 | LBRACE lista_comandos RBRACE
                 | declaracao
                 | SCANF LPAREN STRING_LITERAL COMMA ID RPAREN SEMI
                 | PRINTF LPAREN STRING_LITERAL RPAREN SEMI
                 | PRINTF LPAREN STRING_LITERAL COMMA lista_argumentos RPAREN SEMI
                 | RETURN expressao SEMI
                 | expressao SEMI ;

for_init         : declaracao
                 | expressao SEMI ;

declaracao       : tipo lista_declaracoes SEMI ;
lista_declaracoes: declaracao_variavel
                 | lista_declaracoes COMMA declaracao_variavel ;
declaracao_variavel
                 : ID
                 | ID ASSIGN expressao ;

tipo             : INT | FLOAT | CHAR ;

expressao        : expressao ASSIGN expressao
                 | expressao OR expressao   | expressao AND expressao
                 | expressao EQ expressao   | expressao NEQ expressao
                 | expressao LT expressao   | expressao GT expressao
                 | expressao LE expressao   | expressao GE expressao
                 | expressao PLUS expressao | expressao MINUS expressao
                 | expressao TIMES expressao| expressao DIVIDE expressao
                 | expressao MOD expressao
                 | NOT expressao
                 | MINUS expressao %prec UMINUS
                 | LPAREN expressao RPAREN
                 | ID
                 | ID LPAREN argumentos_opt RPAREN
                 | ID INC | ID DEC | INC ID | DEC ID
                 | NUM | NUM_FLOAT | CHAR_LITERAL | STRING_LITERAL ;

argumentos_opt   : /* vazio */
                 | lista_argumentos ;
lista_argumentos : expressao
                 | lista_argumentos COMMA expressao ;
``````````

Por simplicidade de leitura, foram omitidas acima as regras auxiliares sem símbolos próprios: `bloco_inicio: LBRACE` / `bloco_fim: RBRACE` e `for_inicio: FOR`. Elas existem só para abrir e fechar escopos na tabela de símbolos no momento certo do parse.

## 3. Precedência e associatividade

Declaradas em `parser.y`, da **menor** para a **maior** precedência:

| Declaração Bison | Operadores | Para que serve |
|---|---|---|
| `%nonassoc LOWER_THAN_ELSE` / `%nonassoc ELSE` | — | Resolve o *dangling else*: o `else` se liga ao `if` mais próximo |
| `%right ASSIGN` | `=` | Atribuição encadeada `a = b = 1` |
| `%left OR` | `\|\|` | |
| `%left AND` | `&&` | |
| `%left EQ NEQ` | `== !=` | |
| `%left LT GT LE GE` | `< > <= >=` | |
| `%left PLUS MINUS` | `+ -` | |
| `%left TIMES DIVIDE MOD` | `* / %` | |
| `%right NOT` | `!` | |
| `%right UMINUS` | `-` unário | Usado via `%prec UMINUS` |

## 4. Diferenças entre esta gramática e a EBNF da Especificação

A regra única `expressao` do Bison aceita mais do que a EBNF permite. O excedente é barrado depois:

| O parser aceita | Quem rejeita | Como |
|---|---|---|
| Alvo de atribuição que não é variável (`5 = x`) | Interpretador | `Erro de execucao: o alvo de uma atribuicao precisa ser uma variavel.` |
| `STRING_LITERAL` em qualquer expressão (`1 + "a"`) | Análise semântica | `operador '+' nao pode ser aplicado a uma string.` |
| `STRING_LITERAL` como argumento de função comum (`f("oi")`) | Análise semântica | `argumento 1 de 'f' incompativel (esperado 'int').` |
| `return` no nível do programa | Análise semântica | `'return' fora de uma funcao.` |

## 5. Mensagens de erro sintático

`%define parse.error verbose` faz o Bison informar o token encontrado e os esperados, por exemplo:

``````````text
Erro sintático na linha 1: syntax error, unexpected SEMI
Erro sintático na linha 1: syntax error, unexpected NUM, expecting SEMI or COMMA
``````````

Ainda não há recuperação de erros (regras com o token `error`): a análise para no primeiro erro sintático.
