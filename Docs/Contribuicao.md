# Guia de Contribuição — Compiladores_1

# Contribuições por integrante
## Resumo

| Integrante | Principais entregas |
|---|---|
| Felipe | AST integrada ao parser, funções e main obrigatória, ambiente de execução e avaliação de expressões, automação de sprints, testes, integração das sprints iniciais |
| Gustavo | Estruturas da AST, tabela de símbolos, regras de if-else/while, for, printf, ++/--, testes |
| Ricardo | Estrutura do projeto, tokens e literais, especificação, execução de if/while, chamada de função com pilha de ambientes, revisão e integração dos PRs, correções finais |
| Allan | Especificação e gramática iniciais, semântica (tipos, return, chamadas), scanf |

## Detalhamento por implementação

### Sprint 0 — Organização e especificação (agosto–setembro)

| Implementação | Quem | Arquivos | O que foi acrescentado |
|---|---|---|---|
| Criação do repositório e estrutura inicial | Ricardo | README.md, LICENSE, lexer/lexel.l, parser/parser.y | Repositório, licença MIT e esqueletos do lexer e do parser |
| Automação de sprints no GitHub Projects | Felipe | .github/workflows/sync-issues-sprints.yml, scripts/sync_issues_to_project.py, issues.csv | Backlog com 56 issues e script (via GraphQL) que cria as issues e preenche o campo Sprint no Projects |
| Guia de contribuição | Ricardo | Docs/Contribuicao.md | Padrão de branches, commits e Pull Requests |
| Especificação do MiniC | Ricardo | Docs/Especificacao.md | Tipos, operadores, precedência, estruturas de controle, funções, E/S e catálogo de erros |
| Especificação base e gramática | Allan | Docs/gramatica_minic.md | Primeira gramática EBNF do MiniC |
| Programas de referência em C | Gustavo | programas_teste/fatorial.c, fibonacci.c, mdc.c | Fatorial, Fibonacci e MDC usados como referência |
| Revisão e integração | Felipe | — | Merge dos PRs da documentação (#3–#6) e dos programas de referência do Gustavo (#64) |

### Sprint 1 — Análise léxica e sintática (setembro)

| Implementação | Quem | Arquivos | O que foi acrescentado |
|---|---|---|---|
| Makefile | Felipe | Makefile | Build com Flex + Bison + GCC em build/ |
| Espaços e comentários | Gustavo | lexer/lexel.l | Regras para ignorar espaços, tabulações e quebras de linha |
| Regras de if-else e while | Gustavo | lexer/lexel.l, parser/parser.y | Tokens e produções gramaticais de if, else e while |
| Unificação do lexer e do parser | Felipe | lexer/lexel.l, parser/parser.y, .gitignore | Resolução de conflito de merge; comentários // e /* */ e operadores relacionais e lógicos |
| Palavras-chave e tipos | Ricardo | lexer/lexel.l, parser/parser.y | Tokens int, float, char, return etc. (#16) |
| Declaração de variáveis | Ricardo | lexer/lexel.l, parser/parser.y | Regra de declaração com inicialização opcional (#21) |
| Identificadores e literais | Ricardo | lexer/lexel.l | Identificadores e literais int, float, char e string (#17) |
| Tokens e precedência no parser | Ricardo | parser/parser.y | %token de todos os tokens e %left/%right dos operadores (#20) |
| Revisão e integração | Felipe | — | Merge da gramática do Allan (#67), das regras de espaços e expressões (#68), dos tokens de palavras-chave (#16), das declarações (issue #21) e da integração da sprint na dev (#70) |
| Revisão e integração | Ricardo | — | Merge do lexer e parser de expressões do Felipe (#69) |
| Revisão e integração | Allan | — | Merge da correção da issue #20 (tokens e precedência do Ricardo) |

### Sprint 2 — AST e tabela de símbolos (outubro)

| Implementação | Quem | Arquivos | O que foi acrescentado |
|---|---|---|---|
| Estruturas da AST | Gustavo | src/ast.h, src/ast.c, Makefile | Tipo NoAST, enum de nós e funções criar_no_* |
| Mensagens de erro sintático | Gustavo | lexer/lexel.l, parser/parser.y | yylineno e mensagens detalhadas do Bison |
| Tabela de símbolos | Gustavo | src/tabela.c, src/tabela.h, parser/parser.y | Pilha de escopos aninhados, inserção e busca; integração com as declarações |
| AST nas expressões e comandos | Felipe | parser/parser.y, lexer/lexel.l | Construção da AST em todas as regras (#28, #29) e correção da busca na tabela (#30, #31) |
| Impressão da AST | Felipe | src/ast.c, src/ast.h | imprimir_ast com indentação (#33) |
| Documentação da AST e da tabela | Felipe | Docs/AST_e_Tabela_de_Simbolos.md | Estrutura dos nós, filhos por tipo de nó e escopos (#35) |
| Variável não declarada | Felipe | parser/parser.y | Primeiro erro semântico (#36) |
| Definição e chamada de função | Felipe | parser/parser.y, src/ast.*, src/tabela.* | Nós de função e chamada; assinatura (aridade e tipos dos parâmetros) na tabela (#39) |
| main obrigatória | Felipe | parser/parser.y | iniciar_funcao (escopo de parâmetros) e verificar_main (#43) |
| Semântica do return | Allan | parser/parser.y, src/ast.* | return fora de função e tipo de retorno incompatível |
| Checagem de tipos | Allan | parser/parser.y, src/ast.* | tipo_da_expressao, tipos_compativeis, operações binárias e unárias verificadas, inicialização e atribuição incompatíveis |
| Validação de chamadas | Allan | parser/parser.y | validar_chamada: função inexistente, "não é função", aridade e tipo dos argumentos |
| Revisão e integração | Ricardo | — | Revisão e merge dos PRs #72 e #73 (estruturas e construtores da AST do Gustavo) |
| Revisão e integração | Felipe | — | Merge da tabela de símbolos do Gustavo (#74) e da semântica do Allan (#75) |

### Sprint 3 — Interpretação (novembro)

| Implementação | Quem | Arquivos | O que foi acrescentado |
|---|---|---|---|
| Ambiente de execução e expressões | Felipe | src/ambiente.c, src/ambiente.h, src/interpretador.c, src/interpretador.h, Makefile | Tipo Valor, escopos de execução, conversões e avaliação de todas as expressões (#46) |
| Gramática do printf e do for | Gustavo | lexer/lexel.l, parser/parser.y, src/ast.* | Tokens for, printf e string literal; regras e nós da AST |
| Execução de if, if-else e while | Ricardo | src/interpretador.c | Lógica de execução dos três comandos (#43) |
| Chamada de função na execução | Ricardo | src/interpretador.c | Registro de funções, pilha de ambientes por chamada, return por sinalização e limite de 1.000 chamadas (#51) |
| scanf | Allan | lexer/lexel.l, parser/parser.y, src/ast.*, src/interpretador.c | Token, regra, validação de formato e tipo (%d/%f/%c) e leitura na execução |
| ++ e -- | Gustavo | lexer/lexel.l | Tokens INC/DEC (convertidos em x = x + 1 no parser) |
| for completo e printf na execução | Gustavo | parser/parser.y, src/ast.c, src/interpretador.c | Escopo próprio do for, case NO_FOR e case NO_PRINTF (%d %f %c %s %%, \n, \t) |
| Ajustes no return | Felipe | src/interpretador.c | Estado do return e busca de função (#51) |
| Chamada da main | Ricardo, Gustavo | src/interpretador.c | Chamada da main ao final da execução |
| Revisão e integração | Ricardo | — | Merge do printf/for do Gustavo (#76) e do scanf do Allan (#79) |
| Revisão e integração | Gustavo | — | Merge da execução de if/while do Ricardo (#78) |

### Sprint 4 — Testes e documentação (novembro)

| Implementação | Quem | Arquivos | O que foi acrescentado |
|---|---|---|---|
| Estrutura de testes | Felipe | Testes/DocumentoDeTestes.MD | Pastas por integrante e documento de testes |
| Testes do Felipe | Felipe | Testes/Felipe/*.c, DocumentoDeTestes.MD | 13 testes: declarações, escopo, aritmética, lógicos, conversões, erros semânticos e de execução |
| Testes do Gustavo | Gustavo | Testes/Gustavo/*.c, DocumentoDeTestes.MD | 11 testes: if-else, while, for, printf, ++/--, scanf, escopo do for, programa completo |
| Revisão e integração | Felipe | — | Integração da dev na main com o trabalho das Sprints 2 e 3 de todos os integrantes (#80) |
| Revisão e integração | Ricardo | — | Merge dos testes do Gustavo (#83), das correções (#84, #88), da documentação (#85–#87) e da sincronização final (#89–#92) |

### Correções finais (26–27/11) — geradas com assistência de IA

Commits com código e documentação gerados com auxílio de IA, a partir de uma análise do projeto, e aplicados no repositório pelos integrantes indicados.

| Implementação | Aplicado por | Arquivos | O que foi acrescentado |
|---|---|---|---|
| Execução correta da main | Gustavo | src/interpretador.c | main só executa via executar_funcao (rodava duas vezes) |
| Arquivo por argumento | Gustavo | parser/parser.y | ./build/compilador prog.c, liberando o stdin para o scanf |
| Programa com erro não executa | Gustavo | parser/parser.y, lexer/lexel.l, src/interpretador.* | Contador de erros e código de saída 1; saída = return de main |
| README | Ricardo | README.md | Dependências, build, uso, exemplo e testes |
| Especificação V1.1 e gramática | Ricardo | Docs/Especificacao.md, Docs/gramatica_minic.md | Documentação alinhada ao parser real |
| Decisões técnicas | Ricardo | Docs/decisoes-tecnicas.md | Decisões, registro de problemas, limitações e processo |
| Recuperação de erros sintáticos | Ricardo | parser/parser.y, src/tabela.* | Regras com o token error e ressincronização de escopos |
| make test | Ricardo | scripts/run_tests.sh, Makefile | Comparação via diff com arquivos .esperado |
| Escapes, bloco vazio e linha exata | Ricardo | lexer/lexel.l, parser/parser.y, src/ast.c | '\n' correto, { } aceito e %locations nos erros |
| Opção -ast | Ricardo | parser/parser.y, src/interpretador.*, Makefile | AST e globais só com -ast; remoção do src/main.c vazio |
| Chamada antes da definição e return ausente | Ricardo | parser/parser.y | Chamadas pendentes, recursão mútua e análise de caminhos do return |
| Programas de referência em MiniC | Ricardo | programas_teste/* | Fatorial, Fibonacci e MDC reescritos e incluídos no make test |
| Testes das correções | Ricardo | Testes/Ricardo/*, DocumentoDeTestes.MD | 6 testes: recuperação, linha exata, escapes, blocos vazios, chamada antes da definição, return ausente |

## Sobre o projeto

O **Compiladores_1** é um mini-interpretador de C desenvolvido para a disciplina de Compiladores, utilizando **Flex** (análise léxica) e **Bison** (análise sintática).

## Objetivo

Este guia existe para padronizar a forma como o projeto é versionado e documentado, e principalmente para servir como uma referência rápida de Git para quem ainda não tem familiaridade com a ferramenta — os comandos na seção final podem ser copiados e colados diretamente no terminal.

## Branches

| Branch | Objetivo | Nomenclatura da Branch |
|---|---|---|
| `main` | Abriga o código de produção do projeto, isto é, a versão mais estável e com garantia de funcionamento do mesmo. Todo o conteúdo a ser adicionado nesta deve originar apenas de Pull Requests a partir da branch `dev`. | `main` |
| `dev` | Branch destinada ao código em desenvolvimento. Pode não ser necessariamente estável. O conteúdo a ser adicionado nesta deve ser proveniente de pull requests a partir das branches `feat` ou `doc`. | `dev` |
| `feat` | Branch destinada ao desenvolvimento de funcionalidades. Deve ser associada ao desenvolvimento de alguma funcionalidade, seguindo o proposto pela issue da respectiva funcionalidade. Nesta, podem ser adicionados commits diretamente. Esta deve se originar necessariamente a partir da branch `dev` e deve ser mesclada à mesma. | `feat#número-da-issue/nome-da-issue` |
| `doc` | Branch destinada à documentação. Deve ser associada a uma issue de documentação. Nesta, também podem ser adicionados commits diretamente. Esta deve se originar a partir da branch `dev` e deve ser mesclada à mesma. | `doc#número-da-issue/nome-da-issue` |
| `Aulas` | Branch de trabalho contínuo, onde são feitas as atividades propostas em cada aula da disciplina. Funciona como um "rascunho" evolutivo, separado do fluxo de `feat`/`doc`, para exercícios que não necessariamente viram uma funcionalidade formal. | `Aulas` |

Na prática: atividades de aula em geral entram direto na `Aulas`. Quando um trabalho tem escopo maior — uma funcionalidade nova do interpretador ou uma peça de documentação — ele ganha sua própria branch `feat`/`doc`, é mesclado em `dev` e, quando estável, segue para `main` via Pull Request.

## Commits

Convenção inspirada em [Conventional Commits](https://www.conventionalcommits.org/), adaptada para referenciar o número da issue relacionada à mudança — isso é o que garante a rastreabilidade: qualquer pessoa consegue olhar o histórico e, a partir do número, ir direto na issue do GitHub pra entender o contexto completo daquela mudança.

### Estrutura

```
<tipo>(#numero-da-issue): <breve descrição do que foi feito>
```

Exemplos:

```
feat(#12): implementa reconhecimento de tokens numéricos no lexer
fix(#15): corrige precedência de operadores no parser.y
docs(#3): adiciona guia de contribuição
```

Quando a mudança não estiver ligada a nenhuma issue (ex: ajuste geral, configuração do repositório), o escopo pode ser omitido:

```
chore: atualiza .gitignore
```

### Tipos

| Tipo | Objetivo |
|---|---|
| `feat` | Adiciona alguma funcionalidade ao interpretador (nova regra léxica, nova regra na gramática, etc). |
| `fix` | Corrige um comportamento incorreto. |
| `docs` | Altera documentação (README, este guia, comentários explicativos). |
| `test` | Adiciona ou ajusta arquivos de teste (`tests/valido.txt`, `tests/invalido.txt`, etc). |
| `refactor` | Reorganiza código existente sem mudar o comportamento. |
| `chore` | Tarefa corriqueira, sem impacto direto no funcionamento (configuração, formatação, etc). |

### Recomendações

- Título do commit curto e direto — se precisar explicar mais, use o corpo do commit (linha em branco + parágrafo).
- Um commit deve conter apenas uma mudança coerente — evite misturar, no mesmo commit, uma correção de bug com uma funcionalidade nova.

## Issues

Use as próprias *Issues* do GitHub para registrar bugs, melhorias ou pendências. Sem estimativa de esforço e sem planning poker — para o escopo deste projeto, isso adicionaria burocracia desnecessária.

- Título curto e descritivo.
- Se fizer sentido, use *labels* (`bug`, `enhancement`, `documentação`) para facilitar a busca.
- Descreva o suficiente para que qualquer colega do grupo entenda o que precisa ser feito sem precisar te perguntar.

### Template de BUG

```markdown
## Descrição
<o que está acontecendo de errado>

## Como reproduzir
<passos para reproduzir o problema>

## Contexto adicional
<prints, mensagens de erro, ideias de solução, se houver>
```

### Template de melhoria (enhancement)

```markdown
## Descrição
<o que deveria ser melhorado e por quê>

## Contexto adicional
<referências, exemplos, snippets relevantes>
```

## Pull Requests

Usados principalmente para levar mudanças de `Aulas` para `main`. Sempre que possível, peça para outra pessoa do grupo revisar antes de mesclar.

### Template

```markdown
# Descrição
Este PR adiciona/corrige [...].

# O que foi testado
- [x] Compilação sem erros/warnings
- [x] Casos de teste em tests/valido.txt continuam passando
- [x] Casos de teste em tests/invalido.txt são corretamente rejeitados
```

---

## Cola de Git — comandos prontos para copiar e colar

Esta seção é pra quem trava na hora de usar o Git. Cada bloco abaixo é independente — copie o comando, cole no terminal (dentro da pasta do projeto) e ajuste só o que estiver entre `<>`.

**Configuração inicial (só precisa fazer uma vez por computador):**
```bash
git config --global user.name "Seu Nome"
git config --global user.email "seu-email@exemplo.com"
```

**Clonar o projeto pela primeira vez:**
```bash
git clone https://github.com/<usuario>/Compiladores_1.git
cd Compiladores_1
```

**Ir para a branch de trabalho (Aulas):**
```bash
git checkout Aulas
```

**Trazer as últimas mudanças antes de começar a trabalhar (faça sempre isso primeiro):**
```bash
git pull origin Aulas
```

**Ver o que foi alterado nos seus arquivos:**
```bash
git status
```

**Adicionar suas mudanças para o próximo commit:**
```bash
git add .
```

**Registrar o commit (lembre da convenção `tipo(#numero-da-issue): descrição`):**
```bash
git commit -m "feat(#12): adiciona regra de precedência no parser"
```

**Enviar suas mudanças para o GitHub:**
```bash
git push origin Aulas
```

**Ver o histórico de commits de forma resumida:**
```bash
git log --oneline
```

**Se der conflito ao puxar mudanças (`git pull`):**
1. O Git vai marcar os trechos conflitantes nos arquivos com `<<<<<<<`, `=======` e `>>>>>>>`.
2. Abra o arquivo, decida qual versão manter (ou combine as duas) e apague essas marcações.
3. Depois:
```bash
git add .
git commit -m "fix: resolve conflito de merge"
git push origin Aulas
```

**Se você commitou na branch errada por engano:**
```bash
git log --oneline          # copie o hash do commit
git checkout Aulas          # vá para a branch correta
git cherry-pick <hash-do-commit>
```

## Referências

- [Conventional Commits](https://www.conventionalcommits.org/)
- [Documentação oficial do Git](https://git-scm.com/doc)
- [GNU Flex Manual](https://westes.github.io/flex/manual/)
- [GNU Bison Manual](https://www.gnu.org/software/bison/manual/)
