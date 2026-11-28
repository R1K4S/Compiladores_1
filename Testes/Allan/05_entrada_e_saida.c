// Teste 5: Entrada e saida (printf e scanf)
// Como usa scanf, execute passando entrada pelo stdin:
//   printf "25\n8.5\nK\n" | ./build/compilador 05_entrada_e_saida.c

int main() {
    int idade;
    float nota;
    char inicial;

    printf("Teste de escapes:\ttabulacao e 100%% valido\n");
    printf("Texto via %%s: %s\n", "Ola, MiniC!");

    printf("Digite um inteiro, um float e um char: ");
    scanf("%d", idade);
    scanf("%f", nota);
    scanf("%c", inicial);

    printf("Lidos -> int: %d | float: %f | char: %c\n", idade, nota, inicial);

    return 0;
}
