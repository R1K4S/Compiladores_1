int main() {
    /* Calcula a media de dois valores manualmente (sem funcao, pois return ainda nao executa) */
    int nota1 = 7;
    int nota2 = 9;
    float media = (nota1 + nota2) / 2.0;

    if (media >= 6.0) {
        printf("Aprovado! Media: %f\n", media);
    } else {
        printf("Reprovado! Media: %f\n", media);
    }
    /* esperado: Aprovado! Media: 8.000000 */

    /* for contando repeticoes */
    int i;
    for (i = 0; i < 3; i++) {
        printf("Repeticao numero %d\n", i);
    }
    /* esperado: Repeticao numero 0 / 1 / 2 */

    /* while com contador */
    int contador = 0;
    while (contador < 2) {
        printf("Contador: %d\n", contador);
        contador++;
    }
    /* esperado: Contador: 0 / Contador: 1 */

    /* operacoes mistas int/float */
    int a = 5;
    float b = 2.0;
    float c = a + b;
    printf("5 + 2.0 = %f\n", c);
    /* esperado: 5 + 2.0 = 7.000000 */

    int inteiro = 5 / 2;
    float flutuante = 5 / 2;
    float flutuante2 = 5.0 / 2;
    printf("5/2 int=%d  5/2 float=%f  5.0/2=%f\n", inteiro, flutuante, flutuante2);
    /* esperado: 5/2 int=2  5/2 float=2.000000  5.0/2=2.500000 */

    return 0;
}
