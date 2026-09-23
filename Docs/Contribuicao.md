# Guia de Contribuição — Compiladores_1

## Versionamento

| Versão | Data | Modificação | Responsável |
|---|---|---|---|
| | | | |

_Preencha esta tabela a cada alteração relevante feita neste guia._

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
