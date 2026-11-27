// Fatorial - versao em MiniC do programa de referencia
// Le n e imprime n! calculado com um laco for.
// Diferencas para a versao em C: sem #include, scanf sem '&' e 'int' no lugar de
// 'unsigned long long' (o MiniC so tem int, float e char): o maior fatorial exato e 12!.

int main() {
    int n;
    int i;
    int fatorial = 1;

    printf("Digite um numero inteiro positivo: ");
    scanf("%d", n);

    if (n < 0) {
        printf("Erro! Fatorial de numero negativo nao existe.\n");
    } else {
        for (i = 1; i <= n; ++i) {
            fatorial = fatorial * i;
        }
        printf("Fatorial de %d = %d\n", n, fatorial);
    }

    return 0;
}
