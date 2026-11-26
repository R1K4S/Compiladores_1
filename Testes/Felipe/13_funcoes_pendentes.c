// Teste 13 - Funções (pendente: depende da issue #51)

int dobro(int x) {
    return x * 2;
}

int fatorial(int n) {
    if (n <= 1) {
        return 1;
    } else {
        return n * fatorial(n - 1);
    }
}

int muda(int a) {
    a = 99;
    return a;
}

int main() {
    printf("dobro de 4: %d\n", dobro(4));
    printf("fatorial de 5: %d\n", fatorial(5));

    int v = 1;
    int r = muda(v);
    printf("passagem por valor: v=%d r=%d\n", v, r);
    return 0;
}
