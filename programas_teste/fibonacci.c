// Fibonacci - versao em MiniC do programa de referencia
// Le n e imprime o n-esimo termo da sequencia (fibonacci(0) = 0, fibonacci(1) = 1).
// A versao em C usava memoizacao num vetor 'long int memo[81]'; o MiniC nao tem
// vetores nem 'long', entao o calculo e iterativo, guardando so os dois ultimos termos.
// Com int, o maior termo exato e fibonacci(46).

int fibonacci(int n) {
    int anterior = 0;
    int atual = 1;
    int proximo;
    int i;

    if (n == 0) {
        return 0;
    }
    for (i = 2; i <= n; i++) {
        proximo = anterior + atual;
        anterior = atual;
        atual = proximo;
    }
    return atual;
}

int main() {
    int n;
    scanf("%d", n);
    printf("%d\n", fibonacci(n));
    return 0;
}
