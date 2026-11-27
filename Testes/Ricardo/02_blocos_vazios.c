// Teste - Blocos e corpos de funcao vazios (correcao B6)
// { } deve ser aceito como bloco e como corpo de if/else/while/for.
// (Um corpo de funcao vazio tambem e aceito pela gramatica, mas uma funcao
// int precisa de return - Especificacao 8.2 - por isso nada tem bloco vazio + return.)

int nada() { { } return 0; }

int main() {
    int i;
    if (1) { } else { printf("nao deveria imprimir\n"); }
    while (0) { }
    for (i = 0; i < 3; i++) { }
    { }
    printf("i depois do for vazio: %d (esperado 3)\n", i);
    printf("funcao vazia devolve: %d (esperado 0)\n", nada());
    return 0;
}
