// Teste 4: Funcoes personalizadas, passagem por valor, return, chamada pendente e recursao

// Declaracao adiantada via chamada: funcao definida abaixo de main (chamada pendente)
int somar(int a, int b) {
    return a + b;
}

float media(float x, float y) {
    return (x + y) / 2.0;
}

char escolher_letra(int opcao) {
    if (opcao == 1) {
        return 'S';
    } else {
        return 'N';
    }
}

// Passagem por valor: alterar o parametro nao muda a variavel do chamador
int tentar_alterar(int n) {
    n = n + 100;
    return n;
}

// Recursao direta: fatorial e fibonacci
int fatorial(int n) {
    if (n <= 1) {
        return 1;
    }
    return n * fatorial(n - 1);
}

int fibonacci(int n) {
    if (n <= 0) {
        return 0;
    } else if (n == 1) {
        return 1;
    }
    return fibonacci(n - 1) + fibonacci(n - 2);
}

int main() {
    printf("Soma: %d\n", somar(12, 30));
    printf("Media: %f\n", media(7, 8));
    printf("Char: %c / %c\n", escolher_letra(1), escolher_letra(0));

    int original = 5;
    int alterado = tentar_alterar(original);
    printf("Passagem por valor: original=%d, retorno=%d\n", original, alterado);

    printf("Fatorial(5): %d\n", fatorial(5));
    printf("Fibonacci(7): %d\n", fibonacci(7));

    // Recursao mutua (funcoes definidas abaixo de main)
    printf("eh_par(6)=%d, eh_impar(6)=%d\n", eh_par(6), eh_impar(6));

    return 0;
}

int eh_par(int n) {
    if (n == 0) {
        return 1;
    }
    return eh_impar(n - 1);
}

int eh_impar(int n) {
    if (n == 0) {
        return 0;
    }
    return eh_par(n - 1);
}
