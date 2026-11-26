// Teste 04 - Operadores relacionais e lógicos (resultado é 1 ou 0)

printf("1 < 2: %d (esperado 1)\n", 1 < 2);
printf("1 > 2: %d (esperado 0)\n", 1 > 2);
printf("2 <= 2: %d (esperado 1)\n", 2 <= 2);
printf("3 >= 4: %d (esperado 0)\n", 3 >= 4);
printf("1 == 1: %d (esperado 1)\n", 1 == 1);
printf("1 != 1: %d (esperado 0)\n", 1 != 1);

printf("1 && 0: %d (esperado 0)\n", 1 && 0);
printf("1 || 0: %d (esperado 1)\n", 1 || 0);
printf("!1: %d (esperado 0)\n", !1);
printf("1 || 0 && 0: %d (esperado 1, o && vem antes do ||)\n", 1 || 0 && 0);

float f = 0.5;
printf("float menor que 1: %d (esperado 1)\n", f < 1);

// Curto-circuito: se o resultado já está decidido, o lado direito nem roda.
int x = 0;
int r1 = (0 && (x = 1));
printf("&& com esquerda falsa: r=%d x=%d (esperado r=0 x=0)\n", r1, x);
int r2 = (1 || (x = 1));
printf("|| com esquerda verdadeira: r=%d x=%d (esperado r=1 x=0)\n", r2, x);

int main() { return 0; }