int main() {
    /* while simples: conta de 0 ate 4 */
    int i = 0;
    while (i < 5) {
        printf("%d\n", i);
        i = i + 1;
    }
    /* saida esperada: 0 1 2 3 4 (um por linha) */

    /* while que nao executa nenhuma vez */
    int j = 10;
    while (j < 5) {
        printf("nao deveria imprimir\n");
        j = j + 1;
    }
    printf("while zero vezes ok\n");

    /* while aninhado: tabela 2x3 */
    int lin = 0;
    while (lin < 2) {
        int col = 0;
        while (col < 3) {
            printf("lin=%d col=%d\n", lin, col);
            col = col + 1;
        }
        lin = lin + 1;
    }
    /* saida esperada: lin=0 col=0 / lin=0 col=1 / lin=0 col=2 / lin=1 col=0 / ... */

    /* acumula soma com while */
    int soma = 0;
    int k = 1;
    while (k <= 10) {
        soma = soma + k;
        k = k + 1;
    }
    printf("soma 1..10 esperado: 55  obtido: %d\n", soma);

    return 0;
}
