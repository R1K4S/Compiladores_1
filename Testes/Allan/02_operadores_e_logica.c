// Teste 2: Operadores aritmeticos, relacionais, logicos (&&, ||, !), precedencia e ++/--

int efeito_colateral = 0;

int marcar_chamada(int retorno) {
    efeito_colateral = efeito_colateral + 1;
    return retorno;
}

int main() {
    // Aritmeticos e menos unario
    int x = 15, y = 4;
    printf("Soma: %d\n", x + y);
    printf("Subtracao: %d\n", x - y);
    printf("Multiplicacao: %d\n", x * y);
    printf("Divisao inteira: %d\n", x / y);
    printf("Resto: %d\n", x % y);
    printf("Menos unario: %d\n", -x);

    // Precedencia: * antes de +, relacional antes de &&, && antes de ||
    int prec1 = 2 + 3 * 4;
    int prec2 = (2 + 3) * 4;
    int prec3 = 1 || 0 && 0;     // && tem precedencia maior que || -> 1
    printf("Precedencia: %d, %d, %d\n", prec1, prec2, prec3);

    // Relacionais
    printf("Relacionais: ==:%d !=:%d <:%d >:%d <=:%d >=:%d\n",
           x == 15, x != 15, y < x, y > x, x <= 15, y >= 5);

    // Logicos (AND, OR, NOT)
    int v = 1, f = 0;
    printf("Logicos: &&:%d ||:%d !v:%d !f:%d\n",
           v && f, v || f, !v, !f);

    // Curto-circuito (short-circuit): marcar_chamada nao deve rodar no segundo lado
    efeito_colateral = 0;
    int r1 = 0 && marcar_chamada(1);
    int r2 = 1 || marcar_chamada(1);
    printf("Curto-circuito: r1=%d, r2=%d, chamadas=%d\n", r1, r2, efeito_colateral);

    // Agora o segundo lado precisa ser avaliado
    int r3 = 1 && marcar_chamada(1);
    printf("Sem curto-circuito: r3=%d, chamadas=%d\n", r3, efeito_colateral);

    // Atribuicao encadeada e em expressao (V1.1)
    int a, b, c;
    a = b = 10;
    c = (a = a + 5) * 2;
    printf("Atribuicao encadeada: a=%d, b=%d, c=%d\n", a, b, c);

    // Incremento e decremento (++ e --)
    int i = 5;
    i++;
    ++i;
    printf("Apos ++ duas vezes: i=%d\n", i);
    i--;
    --i;
    printf("Apos -- duas vezes: i=%d\n", i);

    return 0;
}
