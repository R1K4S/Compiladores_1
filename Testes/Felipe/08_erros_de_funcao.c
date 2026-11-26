// Teste 08 - Erros semânticos de função, return e main

int soma(int a, int b) { return a + b; }
int letra(char c) { return 1; }

int r1 = soma(1);
int r2 = letra(10);
int r3 = inexistente(2);
int numero = 4;
int r4 = numero(2);

int soma(int x, int y) { return x; }
int repetido(int a, int a) { return a; }
int errado() { return 'a'; }

return 0;

float main() { return 1.0; }
