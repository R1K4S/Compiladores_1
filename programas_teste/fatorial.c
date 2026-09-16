#include <stdio.h>

int main() {
    int n, i;
    unsigned long long fatorial = 1;

    printf("Digite um numero inteiro positivo: ");
    scanf("%d", &n);

    if (n < 0) {
        printf("Erro! Fatorial de numero negativo nao existe.\n");
    } else {
        for (i = 1; i <= n; ++i) {
            fatorial *= i;
        }
        printf("Fatorial de %d = %llu\n", n, fatorial);
    }

    return 0;
}