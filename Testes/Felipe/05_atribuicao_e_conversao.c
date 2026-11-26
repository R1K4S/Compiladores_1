// Teste 05 - Atribuição e conversão entre int e float

int a = 1;
a = 2;
printf("atribuicao simples: %d (esperado 2)\n", a);

a = a * a + 1;
printf("usando a propria variavel: %d (esperado 5)\n", a);

int b, c;
b = c = 7;
printf("atribuicao encadeada: %d %d (esperado 7 7)\n", b, c);

int d;
int e = (d = 4) + 1;
printf("valor de uma atribuicao: d=%d e=%d (esperado d=4 e=5)\n", d, e);

// Conversões
float f = 10.8;
int i = f;
printf("float para int corta a parte decimal: %d (esperado 10)\n", i);

int j = 3;
float k = j;
printf("int para float: %f (esperado 3.000000)\n", k);

float m = 5 / 2;   // a divisão é feita em int ANTES de virar float
printf("5 / 2 guardado em float: %f (esperado 2.000000)\n", m);

int main() { return 0; }