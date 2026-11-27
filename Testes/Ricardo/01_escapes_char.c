// Teste - Escapes em literais char (correcao B5)
// '\n', '\t', '\0', '\\' e '\'' devem virar o caractere certo, e nao '\'.

int main() {
    char nl = '\n';
    char tab = '\t';
    char zero = '\0';
    char barra = '\\';
    char aspa = '\'';
    printf("codigos: %d %d %d %d %d (esperado 10 9 0 92 39)\n", nl + 0, tab + 0, zero + 0, barra + 0, aspa + 0);
    printf("a%cb%cc\n", tab, nl);
    printf("barra=%c aspa=%c\n", barra, aspa);
    return 0;
}
