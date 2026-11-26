// Teste 02 - Escopo de variáveis (blocos e sombreamento)

int x = 1;

{
    int y = 2;
    printf("bloco enxerga a variavel de fora: %d %d (esperado 1 2)\n", x, y);
}

{
    int x = 10;   // esta x esconde a x de fora, só dentro do bloco
    printf("x dentro do bloco: %d (esperado 10)\n", x);
}
printf("x depois do bloco: %d (esperado 1)\n", x);

{
    x = 50;       // aqui não tem int na frente, então altera a x de fora
}
printf("x alterada de dentro do bloco: %d (esperado 50)\n", x);

int a = 1;
{
    int b = 2;
    {
        int c = 3;
        printf("tres niveis: %d %d %d (esperado 1 2 3)\n", a, b, c);
    }
}

int main() { return 0; }