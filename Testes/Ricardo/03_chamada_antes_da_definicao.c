// Teste - Chamada antes da definicao e recursao mutua (item 12)
// main chama funcoes definidas mais abaixo, e par/impar se chamam mutuamente.

int main() {
    printf("dobro(21) = %d (esperado 42)\n", dobro(21));
    printf("par(10) = %d, par(7) = %d (esperado 1, 0)\n", par(10), par(7));
    printf("impar(7) = %d (esperado 1)\n", impar(7));
    return 0;
}

int dobro(int x) {
    return x * 2;
}

int par(int n) {
    if (n == 0) {
        return 1;
    }
    return impar(n - 1);
}

int impar(int n) {
    if (n == 0) {
        return 0;
    }
    return par(n - 1);
}
