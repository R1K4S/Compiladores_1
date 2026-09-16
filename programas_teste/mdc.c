#include <stdio.h>
int mdc(int a, int b){
    int temp, i;

    // Trocando os valores entre as variáveis
    if(a > b){
        temp = a;
        a = b;
        b = temp;
    }
    // calculo do mdc
    for(i = a; i > 1 && !(a % i == 0 && b % i == 0); i--){}
    return i;
}

int main(){
    int a, b;

    printf("Digite os dois valores: ");
    scanf("%d %d", &a,&b);
    printf("Mdc (%d , %d) -> funcao mdc basica = %d\n",a,b,mdc(a,b));
    return 0;
}