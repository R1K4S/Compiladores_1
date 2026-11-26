// Teste 06 - Erros semânticos de variável
// Esperado no terminal (as linhas são as deste arquivo):
//   Erro Semantico na linha 12: Variavel 'x' nao declarada.
//   Erro Semantico na linha 14: Variavel 'b' nao declarada.
//   Erro Semantico na linha 19: Variavel 'y' nao declarada.
//   Erro Semantico na linha 22: Variavel 'z' ja declarada neste escopo.
// Depois disso, na execução, aparece "Erro de execucao: variavel 'x' nao declarada."
// e o programa para (o x é o primeiro uso inválido).

int a = 1;

x = 10;                

int c = b + 1;        

{
    int y = 5;
}
printf("%d\n", y);     

int z;
int z;                

int main() { return 0; }