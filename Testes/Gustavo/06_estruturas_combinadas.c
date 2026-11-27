int main() {
    /* if dentro de for: imprime so os pares de 0 a 9 */
    printf("pares de 0 a 9:\n");
    for (int i = 0; i < 10; i++) {
        if (i % 2 == 0) {
            printf("%d\n", i);
        }
    }
    /* esperado: 0 2 4 6 8 */

    /* for dentro de while: busca o primeiro i tal que i*i > 20 */
    int limite = 20;
    int encontrado = 0;
    int n = 1;
    while (encontrado == 0) {
        if (n * n > limite) {
            encontrado = n;
        }
        n++;
    }
    printf("primeiro n com n*n > 20: esperado 5  obtido: %d\n", encontrado);

    /* if-else dentro de while: classifica cada numero */
    printf("classifica 1 a 5:\n");
    int m = 1;
    while (m <= 5) {
        if (m % 2 == 0) {
            printf("%d par\n", m);
        } else {
            printf("%d impar\n", m);
        }
        m++;
    }
    /* esperado: 1 impar / 2 par / 3 impar / 4 par / 5 impar */

    /* escopo: variavel declarada dentro do if nao vaza */
    int total = 0;
    for (int k = 1; k <= 4; k++) {
        if (k > 2) {
            int extra = k * 10;
            total = total + extra;
        }
    }
    printf("total (3*10 + 4*10): esperado 70  obtido: %d\n", total);

    return 0;
}
