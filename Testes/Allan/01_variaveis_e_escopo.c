// Teste 1: Declaracao de variaveis, tipos primitivos, globais, escopo e conversoes

int global_int = 10;
float global_float = 2.5;
char global_char = 'Z';

// Comando solto no nivel do programa (executado antes de main)
printf("Global inicial: %d, %f, %c\n", global_int, global_float, global_char);

int main() {
    // Declaracoes simples, multiplas e sem inicializacao (padrao = 0)
    int a, b = 5, c = 10;
    float f1 = 7.8, f2;
    char letra = 'A';
    char nova_linha = '\n';

    printf("Padrao: a=%d, f2=%f\n", a, f2);
    printf("Multiplas: b=%d, c=%d\n", b, c);

    // Conversoes implicitas int <-> float
    f2 = b;          // int -> float: 5.000000
    a = f1;          // float -> int (trunca, nao arredonda): 7
    printf("Conversoes: f2=%f, a=%d\n", f2, a);

    // Divisao inteira vs divisao float
    int div_int = 5 / 2;
    float div_int_para_float = 5 / 2;
    float div_float = 5.0 / 2;
    printf("Divisoes: 5/2=%d, (float)(5/2)=%f, 5.0/2=%f\n",
           div_int, div_int_para_float, div_float);

    // Char em expressoes aritmeticas (V1.1): 'A' vale 65
    int codigo = letra + 0;
    int proxima = letra + 1;
    printf("Char '%c': codigo=%d, proximo=%d%c", letra, codigo, proxima, nova_linha);

    // Escopo de bloco e sombreamento (shadowing)
    int x = 100;
    printf("Fora do bloco: x=%d\n", x);
    {
        int x = 200;
        int interna = 300;
        printf("Dentro do bloco: x=%d, interna=%d, global=%d\n",
               x, interna, global_int);
    }
    printf("Apos o bloco: x=%d\n", x);

    return 0;
}
