// Teste 01 - Declaração de variáveis
// Cada linha mostra o valor obtido e o valor esperado entre parênteses.

int a = 5;
float b = 2.5;
char c = 'A';
int d;                 // sem valor inicial: começa com 0
int e = 1, f = 2, g;   // várias na mesma linha

printf("int com valor inicial: %d (esperado 5)\n", a);
printf("float com valor inicial: %f (esperado 2.500000)\n", b);
printf("char com valor inicial: %c (esperado A)\n", c);
printf("int sem valor inicial: %d (esperado 0)\n", d);

g = e + f;
printf("varias na mesma linha: %d (esperado 3)\n", g);

int main() { return 0; }