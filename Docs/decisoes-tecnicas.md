# Decisões Técnicas e Registro de Problemas

Este documento registra **por que** o interpretador MiniC foi construído do jeito que está, que alternativas foram consideradas e quais problemas apareceram durante o desenvolvimento, com as soluções adotadas. Ele complementa a [Especificação](Especificacao.md), que diz **o que** a linguagem é, e a documentação da [AST e Tabela de Símbolos](AST_e_Tabela_de_Simbolos.md), que diz **como** as estruturas são organizadas.

## 1. Arquitetura em uma página

```text
programa.c ──> lexel.l (Flex) ──tokens──> parser.y (Bison) ──AST──> interpretador.c ──> saída
                                             │  sintaxe + semântica       │
                                             │  numa única passada        │
                                             ▼                            ▼
                                        tabela.c                     ambiente.c
                                  (escopos durante o parse)    (valores durante a execução)
```

| Fase | Arquivo | Entrada → Saída |
|---|---|---|
| Léxica | `lexer/lexel.l` | texto → tokens (`yylval` com o valor) |
| Sintática + semântica | `parser/parser.y`, `src/ast.c`, `src/tabela.c` | tokens → AST + lista de erros |
| Execução | `src/interpretador.c`, `src/ambiente.c` | AST → efeitos (`printf`/`scanf`) + código de saída |

Se houver **qualquer** erro léxico, sintático ou semântico, a fase de execução não começa (D11).

## 2. Decisões técnicas

### D1. Interpretador, e não compilador

**Decisão:** executar o programa percorrendo a AST, sem gerar código intermediário (TAC) nem código de máquina.

**Por quê:** a disciplina oferece os dois caminhos, e o *Guia – Projeto de um interpretador* (COMP1, semana 01) troca a geração de código pela interpretação da AST (passo 5). Assim a equipe concentrou o esforço nas fases de análise, que são o núcleo da disciplina, e tem um resultado executável mais cedo para testar.

**Consequência:** as práticas das semanas 08 (TAC) e 10 (bitcode) não se aplicam ao projeto. A otimização (semana 09) é opcional e não foi feita.

### D2. Linguagem-fonte: MiniC, um subconjunto de C

**Decisão:** escalares `int`/`float`/`char`, `if`/`while`/`for`, funções com recursão, `printf`/`scanf`. Sem ponteiros, vetores ou `struct`.

**Por quê:** C é a linguagem mais conhecida no curso, e programas MiniC sem `scanf` compilam no `gcc` real. Isso permite conferir a saída esperada antes de o interpretador existir (Especificação, Seção 14). Os recursos excluídos (ponteiros, memória) exigiriam um modelo de memória que não cabe no semestre.

**Consequência:** `printf` e `scanf` são **primitivas da linguagem**, não funções de biblioteca. Por isso o `scanf` do MiniC dispensa o `&` (não há ponteiros) e não precisa de `#include`.

### D3. Gramática do Bison com uma regra `expressao` e declarações de precedência

**Decisão:** em vez de uma regra por nível de precedência (`expressao_aditiva`, `expressao_multiplicativa`...), o `parser.y` tem uma única regra `expressao` com todas as operações. A precedência é resolvida por `%left`/`%right`.

**Por quê:** é a técnica mostrada na semana 05 do COMP1 e gera menos regras e ações duplicadas. A tabela de precedência da Especificação (Seção 5.7) vira diretamente as declarações do Bison. A gramática compila **sem conflitos**.

**Consequência:** o parser aceita algumas formas que a EBNF não permite, como `5 = x` ou uma string no meio de uma conta. Elas são barradas depois, pela semântica ou pelo interpretador ([gramatica_minic.md](gramatica_minic.md), Seção 4).

### D4. *Dangling else* com `%prec LOWER_THAN_ELSE`

**Decisão:** o `if` sem `else` recebe uma precedência menor que o token `ELSE`.

**Por quê:** `if (a) if (b) x; else y;` é ambíguo e causaria um conflito shift/reduce. Com a precedência, o Bison faz o *shift* e o `else` se liga ao `if` mais próximo, como em C. Não existe `else if` como palavra reservada: ele é só um `else` cujo corpo é outro `if`.

### D5. AST com nó genérico

**Decisão:** um único tipo `NoAST`, com um `enum TipoNo`, quatro filhos (`esq`, `dir`, `centro`, `extra`), um `proximo` para listas e uma `union` com o dado do nó.

**Por quê:** todas as funções de impressão, inferência de tipo e interpretação recebem o mesmo tipo e decidem por `switch (no->tipo)`. Listas de comandos, parâmetros e argumentos usam `proximo` com `ligar_nos`, sem uma estrutura de lista separada.

**Consequências:**
- O significado de cada filho depende do tipo do nó (no `for`, `esq` = inicialização, `dir` = condição, `centro` = passo, `extra` = corpo). Essa correspondência está tabelada em [AST_e_Tabela_de_Simbolos.md](AST_e_Tabela_de_Simbolos.md).
- O tipo de retorno de `NO_FUNCAO` fica fora da `union` (campo `tipo_retorno`), porque o nome da função já ocupa `string_val`.

### D6. Análise semântica dentro das ações do parser (uma única passada)

**Decisão:** as checagens semânticas (variável declarada, redeclaração, tipos, aridade, `return`, `scanf`, `main`) rodam nas ações das regras do Bison, no momento em que cada construção é reconhecida. Não há um percurso separado da AST.

**Por quê:**
- A tabela de símbolos precisa abrir e fechar escopos exatamente quando o parser encontra `{` e `}`. As regras auxiliares `bloco_inicio`, `bloco_fim` e `for_inicio` fazem isso sem esforço extra.
- Quando o parser reduz `ID`, o escopo atual da tabela é exatamente o escopo léxico daquele ponto do texto. Num percurso posterior da AST, seria preciso reconstruir os escopos.
- A linha de cada símbolo (`@n`, via `%locations`) está disponível na hora de reportar o erro.

**Consequências:**
- Uma função precisa estar **definida acima** do ponto em que é chamada. Como não há protótipos, recursão mútua não é possível (Especificação, Seção 8.3).
- Para as mensagens apontarem a linha exata, o parser usa `%locations`: o lexer grava a linha de cada token em `yylloc` (`YY_USER_ACTION`), e as ações usam `@n.first_line` do símbolo do erro, em vez de `yylineno`. O `yylineno` já estaria na linha do *lookahead*, às vezes uma linha depois. Os erros **sintáticos** continuam na linha do token inesperado.
- Os tipos são inferidos sob demanda (`tipo_da_expressao`), em vez de anotados nos nós da AST.

**Alternativa considerada:** uma 2ª passada sobre a AST resolveria a ordem das funções, mas exigiria guardar escopos na própria AST. Fica como melhoria futura.

### D7. Tabela de símbolos como pilha de escopos encadeados

**Decisão:** cada escopo é uma lista encadeada de símbolos, com um ponteiro `pai`. A busca vai do escopo atual para fora; a inserção verifica duplicatas só no escopo atual. Funções e variáveis dividem o mesmo espaço de nomes, como em C. Um símbolo de função guarda a aridade e o tipo de cada parâmetro.

**Por quê:** é a estrutura mais simples que implementa escopo léxico, com custo linear desprezível para programas didáticos. Guardar a assinatura permite checar aridade e tipos na chamada sem consultar a AST.

**Consequência:** o sombreamento (*shadowing*) é permitido naturalmente: a redeclaração só é procurada no escopo atual. A Especificação V1 proibia sombreamento; a V1.1 adotou o comportamento implementado, que é o do C.

### D8. Dois ambientes: tabela de símbolos (análise) × ambiente de execução

**Decisão:** a execução não usa a tabela de símbolos. Ela tem estrutura própria (`ambiente.c`): uma pilha de `EscopoExec` com `VarExec` (nome + valor).

**Por quê:**
- A tabela de símbolos **já foi desmontada** quando a execução começa: cada `sair_escopo()` do parse libera os símbolos do bloco.
- Na execução, a mesma função pode estar ativa várias vezes (recursão). Cada chamada precisa de suas próprias variáveis, enquanto na análise existe um único escopo por trecho de texto.
- Cada estrutura guarda o que sua fase precisa: a tabela guarda tipos e assinaturas; o ambiente guarda valores.

**Detalhe importante:** o escopo-base de uma chamada é marcado com `eh_funcao = 1`. Ao subir na busca a partir dele, o ambiente **pula direto para o global**. Assim, uma função não enxerga as variáveis locais de quem a chamou (escopo estático, não dinâmico).

### D9. Valores com tipo em tempo de execução e conversão na escrita

**Decisão:** cada `Valor` carrega o seu tipo (`VAL_INT`, `VAL_FLOAT`, `VAL_CHAR`) e uma `union`. Toda escrita numa variável converte o valor para o tipo da variável (`converter_valor`). O mesmo vale para parâmetros e para o valor de `return`.

**Por quê:** implementa as regras de conversão da Especificação (Seção 6) num único ponto: `float → int` trunca e `int → float` é exato. As operações aritméticas promovem para `float` quando algum operando é `float`; senão, usam aritmética inteira, o que dá a divisão inteira de `5 / 2`.

### D10. `return` por sinalização, e não por `setjmp`/`longjmp`

**Decisão:** um `return` grava `valor_retorno` e liga a flag `retornando`. Os laços e `interpretar_lista` param quando a flag está ligada, e `executar_funcao` a desliga ao sair.

**Por quê:** é simples de seguir e de depurar, não salta por cima da limpeza dos escopos (`ambiente_sair_escopo` sempre roda) e não depende de recursos avançados de C.

**Consequência:** a profundidade de chamadas é limitada a **1.000** (`PROFUNDIDADE_MAXIMA`). Uma recursão infinita gera um erro de execução claro, em vez de estourar a pilha do processo C com *segmentation fault*.

### D11. Programa com erro não executa; código de saída significativo

**Decisão:** um contador global `total_erros` é incrementado por erros léxicos (`lexel.l`), sintáticos (`yyerror`) e semânticos (macro `ERRO_SEMANTICO`). Se for maior que zero, o programa não é executado e o interpretador sai com `1`. Sem erros, o código de saída é o valor retornado por `main`.

**Por quê:** o guia do interpretador (passo 4) pede erros reportados **antes** da execução, e a Especificação (Seção 11) manda rejeitar programas inválidos. Executar um programa semanticamente errado produzia efeitos parciais e erros de execução confusos (ver P2). O código de saída permite automatizar os testes (D14).

### D12. Recuperação de erros sintáticos com o token `error`

**Decisão:** regras `error SEMI` (comando), `... error bloco_fim` (bloco), `... error RBRACE` (corpo de função) e `error RBRACE` (nível do programa), todas com `yyerrok`.

**Por quê:** é a técnica da semana 05 do COMP1. Ela permite reportar vários erros numa execução, em vez de parar no primeiro.

**Problema resolvido junto:** quando o Bison descarta um `}`, o escopo aberto por `bloco_inicio` ficaria aberto na tabela. Por isso o parser chama `restaurar_escopo(NIVEL_GLOBAL)` ao começar cada função e antes de verificar `main`. Funções só existem no nível do programa, então esse é sempre o escopo certo nesses pontos.

**Consequência:** quando falta um `(`, `)` ou `{`, ainda aparecem erros em cascata. É o comportamento comum de compiladores reais; a orientação é corrigir o primeiro erro da lista.

### D13. `++`/`--` como açúcar sintático

**Decisão:** `x++`, `++x`, `x--` e `--x` são transformados, já no parser, em `x = x + 1` / `x = x - 1` (`criar_incremento`).

**Por quê:** o incremento estava no backlog de expansão e é o jeito natural de escrever o passo de um `for`. Como vira um `NO_ATRIBUICAO` comum, o interpretador não precisou de nenhum caso novo, e a checagem de tipos é a mesma da atribuição.

**Consequência:** as quatro formas devolvem o valor **novo**. Em `y = x++;` o MiniC difere do C. Como comando isolado ou no passo do `for`, o efeito é idêntico. Isso está documentado na Especificação V1.1 (Seção 5.6).

### D14. Especificação V1.1: documentar o comportamento implementado

**Decisão:** quando a análise do projeto mostrou que o parser aceitava construções proibidas pela V1 (globais, `++`/`--`, atribuição encadeada, sombreamento, `char` em aritmética), a equipe atualizou a Especificação em vez de remover recursos.

**Por quê:** os recursos funcionavam e tinham testes. Retirá-los exigiria mudar a gramática perto da entrega, com risco de regressão, e não traria ganho ao usuário da linguagem. O importante era a documentação e o código voltarem a dizer a mesma coisa. Cada mudança está marcada com **(V1.1)** no texto.

### D15. Testes por comparação com saída esperada

**Decisão:** `make test` roda cada `Testes/*/*.c` e compara stdout, stderr e o código de saída com um arquivo `.esperado`, via `diff`. Um `.in` opcional alimenta o `scanf`. `make test-atualizar` regrava os `.esperado`.

**Por quê:** segue o `run_tests.sh` das semanas 07–08 e a dica do guia de manter testes automáticos por fase. A conferência deixou de ser visual ("esperado X, obtido Y"): um único comando diz se algo quebrou. stdout e stderr são capturados **separadamente** porque, juntos (`2>&1`), a ordem das linhas depende do tamanho do buffer do sistema e mudaria entre Linux e macOS.

**Consequência:** a saída inclui a impressão da AST, então os testes também cobrem o parser. Uma mudança intencional na impressão exige rodar `make test-atualizar` e revisar o `git diff`.

## 3. Registro de problemas e soluções

| # | Problema | Sintoma | Causa | Solução |
|---|---|---|---|---|
| P1 | `main` executada pelo caminho errado | Todo `return 0;` em `main` terminava em `Erro de execucao: 'return' fora de uma funcao` (saída 1); sem `return`, `main` rodava **duas vezes** | Ao percorrer o nível do programa, o `case NO_FUNCAO` executava o corpo de `main` sem quadro de função; depois `executar_programa` chamava `main` de novo | `NO_FUNCAO` não executa nada no percurso do topo; funções só rodam via `executar_funcao` |
| P2 | Programa com erro semântico era executado | Ex.: `i` usada fora do `for` era acusada, mas o laço rodava e só depois vinha um erro de execução | As checagens só imprimiam a mensagem; `main()` executava sempre que `yyparse()` retornava 0 | Contador `total_erros` + não executar quando há erro (D11) |
| P3 | `scanf` sem entrada | `falha ao ler um inteiro` em qualquer programa com `scanf` | Código-fonte e entrada do usuário dividiam o mesmo `stdin` | Arquivo-fonte passado como argumento: `./build/compilador prog.c` |
| P4 | Código de saída sempre 0 | Scripts não distinguiam sucesso de erro sintático | `main()` do interpretador sempre retornava 0 | Saída 1 em qualquer erro; retorno de `main` em caso de sucesso |
| P5 | Análise parava no primeiro erro sintático | Um erro de digitação por execução | Nenhuma regra com o token `error` | Recuperação de erros (D12) |
| P6 | Documentação divergente do código | `gramatica_minic.md` sem `for`, `%`, `printf`; Especificação proibindo recursos aceitos | A gramática evoluiu ao longo das sprints e os documentos não acompanharam | Especificação V1.1 (D14) e `gramatica_minic.md` reescrita a partir do `parser.y` |
| P7 | Testes conferidos a olho | Regressões passavam despercebidas; `DocumentoDeTestes.MD` marcava como pendente um teste que já passava | Não havia comparação automática | `make test` com `.esperado` (D15) |
| P8 | Escapes em literal `char` | `'\n'` virava `'\\'` (código 92) | O lexer guardava `yytext[1]`, que num escape é a própria barra | Função `valor_char_literal` no `lexel.l` traduz `\n`, `\t`, `\0` e os demais |
| P9 | Bloco vazio rejeitado | `if (x) { }` e `int f() { }` davam erro sintático | `lista_comandos` exigia ao menos 1 comando | Alternativas `bloco_inicio bloco_fim` e `LBRACE RBRACE` (sem conflitos no Bison) |
| P10 | Linha do erro deslocada | Um erro numa expressão que continuava na linha seguinte era acusado nessa linha seguinte | As ações usavam `yylineno`, que já estava no *lookahead* | `%locations` + `@n.first_line` (D6) |

## 4. Limitações conhecidas

| Limitação | Exemplo | Caminho de solução |
|---|---|---|
| Função precisa ser definida antes do uso; sem recursão mútua | `a()` chama `b()`, definida abaixo | Pré-registrar as funções ou fazer uma 2ª passada semântica (D6) |
| Ausência de `return` não é detectada | Função `int` sem `return` devolve 0 | Análise de caminhos na AST (Especificação, Seção 11, item 7) |
| `printf` sem largura/precisão e sem checagem de tipo | `%.2f`; `%d` com `float` | Validar o formato no parser, contra os tipos dos argumentos |
| AST e escopo global não são liberados ao final | — | Função `liberar_ast`; irrelevante para um processo de vida curta |

## 5. Processo

- **Scrum com sprints semanais**, como recomenda o enunciado do trabalho. O backlog está em `issues.csv` (56 itens nas Sprints 1–6, com labels por fase: `lexer`, `parser`, `ast`, `symtab`, `semantic`, `interpreter`, `docs`).
- O workflow `.github/workflows/sync-issues-sprints.yml` (disparado manualmente) usa `scripts/sync_issues_to_project.py` para criar as issues e preencher o campo *Sprint* no GitHub Projects a partir do CSV.
- Branches `main`, `dev`, `feat`/`doc` e `Aulas`, e commits no padrão `tipo(#issue): descrição` ([Contribuicao.md](Contribuicao.md)).
- Os testes são organizados por integrante em `Testes/<nome>/`, e cada um testa ao menos o que implementou ([DocumentoDeTestes.MD](../Testes/DocumentoDeTestes.MD)).
