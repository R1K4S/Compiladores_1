// Teste 3: Estruturas de controle (if, else, else if, while, for, blocos)

int main() {
    // if / else if / else
    float nota = 7.5;
    if (nota >= 9.0) {
        printf("Conceito A\n");
    } else if (nota >= 6.0) {
        printf("Aprovado (nota %f)\n", nota);
    } else {
        printf("Reprovado\n");
    }

    // Dangling else: o else deve se ligar ao if mais proximo (b > 0)
    int a = 0, b = 1;
    if (a > 0)
        if (b > 0)
            printf("Caso 1\n");
        else
            printf("Caso 2 (nao deve imprimir)\n");

    // Bloco vazio e if sem chaves
    if (b == 1) { }
    if (b == 1) printf("If sem chaves funcionou\n");

    // Laco while
    int soma = 0;
    int k = 1;
    while (k <= 5) {
        soma = soma + k;
        k++;
    }
    printf("Soma com while (1..5): %d\n", soma);

    // Laco for com declaracao local de i (escopo restrito ao for)
    int prod = 1;
    for (int i = 1; i <= 4; i++) {
        prod = prod * i;
        printf("for i=%d, prod=%d\n", i, prod);
    }

    // Reutilizando o nome 'i' fora do for (valido porque o 'i' do for saiu de escopo)
    int i = 99;
    printf("Fora do for: i=%d\n", i);

    return 0;
}
