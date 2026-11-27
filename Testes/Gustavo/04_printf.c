int main() {
    int    n  = 42;
    float  f  = 3.14;
    char   c  = 'Z';

    /* %d */
    printf("inteiro esperado: 42  obtido: %d\n", n);

    /* %f */
    printf("float esperado: 3.140000  obtido: %f\n", f);

    /* %c */
    printf("char esperado: Z  obtido: %c\n", c);

    /* %s com string literal */
    printf("string esperado: ola  obtido: %s\n", "ola");

    /* multiplos especificadores na mesma chamada */
    printf("n=%d f=%f c=%c\n", n, f, c);

    /* %% literal */
    printf("cem por cento: 100%%\n");

    /* string sem especificadores */
    printf("linha sem variaveis\n");

    /* expressao como argumento */
    printf("2 + 3 = %d\n", 2 + 3);

    /* negativo */
    printf("negativo esperado: -7  obtido: %d\n", -7);

    return 0;
}
