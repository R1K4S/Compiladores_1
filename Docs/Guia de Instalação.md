# Guia de Instalação do Ambiente

Este guia descreve como preparar o ambiente de desenvolvimento para compilar e executar este projeto, que utiliza **Flex** (análise léxica), **Bison** (análise sintática) e **C** (compilação via `gcc`/`make`).

## Pré-requisitos

Antes de começar, certifique-se de ter as seguintes ferramentas instaladas:

| Ferramenta | Finalidade |
|---|---|
| `gcc` | Compilador C |
| `make` | Automação da build (usa o `Makefile` do projeto) |
| `flex` | Gerador do analisador léxico (`lexer/lexel.l`) |
| `bison` | Gerador do analisador sintático (`parser/parser.y`) |
| `git` | Clonar o repositório |

---

## 1. Instalação por Sistema Operacional

### Linux (Ubuntu/Debian)

```bash
sudo apt update
sudo apt install -y build-essential flex bison git
```

`build-essential` já inclui `gcc` e `make`.

### Windows (via WSL)

1. Instale o WSL (caso ainda não tenha):
   ```powershell
   wsl --install
   ```
   Reinicie o computador se solicitado e configure sua distribuição Linux (recomendado: Ubuntu).

2. Dentro do terminal WSL (Ubuntu), siga os mesmos passos da seção **Linux (Ubuntu/Debian)** acima:
   ```bash
   sudo apt update
   sudo apt install -y build-essential flex bison git
   ```

> Não é recomendado compilar diretamente no Windows nativo (via MSYS2/MinGW) sem adaptações no `Makefile`. O WSL garante compatibilidade total com o ambiente Linux usado no projeto.

### macOS

1. Instale o [Homebrew](https://brew.sh/) (caso ainda não tenha):
   ```bash
   /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
   ```

2. Instale as dependências:
   ```bash
   brew install flex bison git
   ```

>  O macOS já vem com `clang` como `gcc` por padrão e com `make` via Xcode Command Line Tools. Se não estiverem instalados, rode:
   ```bash
   xcode-select --install
   ```

>  O `flex`/`bison` do macOS (versões do sistema) costumam ser antigos. O Homebrew instala versões mais recentes, mas pode ser necessário ajustar o `PATH`:
   ```bash
   echo 'export PATH="/usr/local/opt/flex/bin:/usr/local/opt/bison/bin:$PATH"' >> ~/.zshrc
   source ~/.zshrc
   ```
   (em Macs com Apple Silicon, o caminho costuma ser `/opt/homebrew/opt/...`)

---

## 2. Verificando as instalações

Rode os comandos abaixo para confirmar que tudo está corretamente instalado:

```bash
gcc --version
make --version
flex --version
bison --version
git --version
```

Todos devem retornar um número de versão sem erros.

---

## 3. Clonando o repositório

```bash
git clone <URL_DO_REPOSITORIO>
cd Compiladores_1
```

---

## 4. Compilando o projeto

Com o `Makefile` na raiz do projeto, basta rodar:

```bash
make
```

Isso deve gerar o executável do compilador a partir de `lexer/lexel.l`, `parser/parser.y` e `src/main.c`.

Para limpar os arquivos gerados pela build (caso o `Makefile` possua esse alvo):

```bash
make clean
```

---

## 5. Executando

```bash
./<nome_do_executavel_gerado>
```

> Consulte o `README.md` do projeto para o nome exato do executável e exemplos de uso.

---

## Problemas comuns

| Erro | Possível causa | Solução |
|---|---|---|
| `flex: command not found` | Flex não instalado ou não no PATH | Reinstale seguindo a seção do seu SO |
| `bison: command not found` | Bison não instalado ou não no PATH | Reinstale seguindo a seção do seu SO |
| Erros de sintaxe do `parser.y` na build | Versão do Bison incompatível | Verifique a versão com `bison --version` e compare com a exigida no projeto |
| `make: *** No rule to make target` | Executado fora da raiz do projeto | Certifique-se de estar na pasta que contém o `Makefile` |