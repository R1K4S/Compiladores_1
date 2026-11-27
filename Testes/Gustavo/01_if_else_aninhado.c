int main() {
    int nota = 75;

    /* else-if em cadeia: so um ramo executa */
    if (nota >= 90) {
        printf("resultado esperado: A  obtido: %d\n", nota);
    } else {
        if (nota >= 70) {
            printf("resultado esperado: B  obtido: %d\n", nota);
        } else {
            if (nota >= 50) {
                printf("resultado esperado: C  obtido: %d\n", nota);
            } else {
                printf("resultado esperado: F  obtido: %d\n", nota);
            }
        }
    }

    /* if-else aninhado no ramo then */
    int x = 5;
    int y = 10;
    if (x < y) {
        if (x == 5) {
            printf("x < y e x == 5: esperado 1  obtido: 1\n");
        } else {
            printf("x < y e x != 5: esperado 0  obtido: 0\n");
        }
    } else {
        printf("ramo errado: nao deveria imprimir\n");
    }

    /* condicao falsa: else deve executar */
    int a = 0;
    if (a) {
        printf("ramo errado: nao deveria imprimir\n");
    } else {
        printf("a e zero, else executou: esperado 1  obtido: 1\n");
    }

    return 0;
}
