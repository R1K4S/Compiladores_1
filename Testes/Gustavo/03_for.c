int main() {
    /* for com declaracao no init */
    int soma = 0;
    for (int i = 0; i < 5; i++) {
        soma = soma + i;
    }
    printf("soma 0..4 esperado: 10  obtido: %d\n", soma);

    /* for com expressao no init (variavel ja declarada) */
    int j = 0;
    for (j = 1; j <= 3; j++) {
        printf("j=%d\n", j);
    }
    /* saida esperada: j=1 j=2 j=3 */

    /* for que nao executa (condicao falsa desde o inicio) */
    for (int k = 10; k < 5; k++) {
        printf("nao deveria imprimir\n");
    }
    printf("for zero iteracoes ok\n");

    /* for aninhado: tabuada do 2 e do 3 */
    for (int a = 2; a <= 3; a++) {
        for (int b = 1; b <= 3; b++) {
            printf("%d x %d = %d\n", a, b, a * b);
        }
    }
    /*
     saida esperada:
     2 x 1 = 2
     2 x 2 = 4
     2 x 3 = 6
     3 x 1 = 3
     3 x 2 = 6
     3 x 3 = 9
    */

    /* variavel do for nao deve vazar para fora */
    int vazou = 0;
    for (int v = 0; v < 3; v++) {
        vazou = v;
    }
    printf("ultimo valor de vazou esperado: 2  obtido: %d\n", vazou);

    return 0;
}
