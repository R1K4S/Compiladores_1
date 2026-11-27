int main() {
    float f;
    int   n;

    /* formato invalido para scanf */
    scanf("%s", f);

    /* tipo incompativel: formato %d mas variavel e float */
    scanf("%d", f);

    /* tipo incompativel: formato %f mas variavel e int */
    scanf("%f", n);

    /* variavel nao declarada no scanf */
    scanf("%d", fantasma);

    return 0;
}
