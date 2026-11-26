// Teste 03 - Operadores aritméticos, precedência e divisão

int a = 10;
int b = 3;

printf("soma: %d (esperado 13)\n", a + b);
printf("subtracao: %d (esperado 7)\n", a - b);
printf("multiplicacao: %d (esperado 30)\n", a * b);
printf("divisao inteira: %d (esperado 3)\n", a / b);
printf("resto: %d (esperado 1)\n", a % b);

printf("multiplicacao antes da soma: %d (esperado 14)\n", 2 + 3 * 4);
printf("parenteses: %d (esperado 20)\n", (2 + 3) * 4);
printf("expressao grande: %d (esperado 5)\n", (3 * 3 + 4 * 4) / 5);

printf("menos unario: %d (esperado -10)\n", -a);

printf("divisao com float: %f (esperado 3.500000)\n", 7 / 2.0);
printf("int com float: %f (esperado 7.000000)\n", 5 + 2.0);

int main() { return 0; }