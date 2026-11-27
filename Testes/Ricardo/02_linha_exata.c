// Teste - Linha exata nos erros semanticos (correcao B8, %locations)
// Cada erro deve apontar a linha do proprio simbolo, mesmo quando a
// expressao continua na linha seguinte.

int main() {
    int x = 1;
    x = y
        + 1;
    char c = 1
        + 2;
    int z = 2
        + "a";
    return 0;
}
