#!/usr/bin/env bash
# =========================================================
# Testes automatizados do interpretador MiniC
#
# Roda cada Testes/<pasta>/<teste>.c e compara a saida com
# Testes/<pasta>/<teste>.esperado (no estilo do run_tests.sh
# das semanas 07-08 do COMP1, mas comparando com diff).
#
#   make test             -> compara (sai com 1 se algum teste falhar)
#   make test-atualizar   -> regrava todos os .esperado com a saida atual
#
# Se existir Testes/<pasta>/<teste>.in, ele e usado como entrada (scanf).
# O .esperado guarda: stdout, depois stderr, depois o codigo de saida.
# =========================================================
set -u

RAIZ="$(cd "$(dirname "$0")/.." && pwd)"
BIN="$RAIZ/build/compilador"
cd "$RAIZ" || exit 2

if [ ! -x "$BIN" ]; then
    echo "Executavel $BIN nao encontrado. Rode 'make' antes."
    exit 2
fi

MODO="comparar"
if [ "${1:-}" = "--atualizar" ]; then
    MODO="atualizar"
fi

TMP_ERR="$(mktemp)"
TMP_OBTIDO="$(mktemp)"
trap 'rm -f "$TMP_ERR" "$TMP_OBTIDO"' EXIT

passou=0
falhou=0
falhas=""

shopt -s nullglob
for prog in Testes/*/*.c; do
    esperado="${prog%.c}.esperado"
    entrada="${prog%.c}.in"
    [ -f "$entrada" ] || entrada="/dev/null"

    # stdout e stderr capturados separadamente: a ordem fica deterministica
    saida="$("$BIN" "$prog" < "$entrada" 2> "$TMP_ERR")"
    codigo=$?
    {
        printf '%s\n' "$saida"
        printf -- '--- stderr ---\n'
        cat "$TMP_ERR"
        printf -- '--- exit: %s ---\n' "$codigo"
    } > "$TMP_OBTIDO"

    if [ "$MODO" = "atualizar" ]; then
        cp "$TMP_OBTIDO" "$esperado"
        printf 'GRAVADO  %-48s exit=%s\n' "$prog" "$codigo"
        continue
    fi

    if [ ! -f "$esperado" ]; then
        printf 'FALHOU   %-48s (sem %s; rode make test-atualizar)\n' "$prog" "$(basename "$esperado")"
        falhou=$((falhou + 1))
        falhas="$falhas $prog"
    elif diff -u "$esperado" "$TMP_OBTIDO" > /dev/null; then
        printf 'OK       %-48s exit=%s\n' "$prog" "$codigo"
        passou=$((passou + 1))
    else
        printf 'FALHOU   %-48s exit=%s\n' "$prog" "$codigo"
        diff -u "$esperado" "$TMP_OBTIDO" | head -n 30 | sed 's/^/         /'
        falhou=$((falhou + 1))
        falhas="$falhas $prog"
    fi
done

if [ "$MODO" = "atualizar" ]; then
    echo
    echo "Arquivos .esperado regravados. Confira com 'git diff Testes/' antes de commitar."
    exit 0
fi

echo
echo "Resultado: $passou passaram, $falhou falharam."
if [ "$falhou" -gt 0 ]; then
    echo "Falharam:$falhas"
    exit 1
fi
exit 0
