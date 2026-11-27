int main() {
    int x = 5;

    /* pos-incremento: x passa de 5 para 6 */
    x++;
    printf("apos x++: esperado 6  obtido: %d\n", x);

    /* pre-incremento */
    ++x;
    printf("apos ++x: esperado 7  obtido: %d\n", x);

    /* pos-decremento */
    x--;
    printf("apos x--: esperado 6  obtido: %d\n", x);

    /* pre-decremento */
    --x;
    printf("apos --x: esperado 5  obtido: %d\n", x);

    /* dentro de for com ++ no incremento */
    int soma = 0;
    for (int i = 1; i <= 5; i++) {
        soma = soma + i;
    }
    printf("soma 1..5 com i++: esperado 15  obtido: %d\n", soma);

    /* dentro de for com -- (contagem regressiva) */
    int prod = 1;
    for (int j = 5; j >= 1; j--) {
        prod = prod * j;
    }
    printf("5! com j--: esperado 120  obtido: %d\n", prod);

    /* ++ em variavel float */
    float f = 2.5;
    f++;
    printf("2.5 apos f++: esperado 3.500000  obtido: %f\n", f);

    return 0;
}
