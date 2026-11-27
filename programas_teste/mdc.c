// MDC - versao em MiniC do programa de referencia
// Le dois valores e imprime o maior divisor comum pelo metodo basico:
// testa divisores de min(a, b) para baixo ate achar um que divida os dois.
// Diferencas para a versao em C: sem #include, um scanf por variavel e sem '&',
// e um while no lugar do for com corpo vazio.

int mdc(int a, int b) {
    int temp;
    int i;

    // Trocando os valores entre as variaveis
    if (a > b) {
        temp = a;
        a = b;
        b = temp;
    }
    // calculo do mdc
    i = a;
    while (i > 1 && !(a % i == 0 && b % i == 0)) {
        i--;
    }
    return i;
}

int main() {
    int a;
    int b;

    printf("Digite os dois valores: ");
    scanf("%d", a);
    scanf("%d", b);
    printf("Mdc (%d , %d) -> funcao mdc basica = %d\n", a, b, mdc(a, b));
    return 0;
}
