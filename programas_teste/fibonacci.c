#include <stdio.h>

long int memo[81];

long int fibonacci(int n){
    if(n == 0) return 0;
    if(n == 1 || n == 2) return 1;
    if(memo[n] != 0) return memo[n];
    memo[n] = fibonacci (n-1) + fibonacci(n-2);
    return memo[n];
}
int main(){
    int n;
    scanf("%d", &n);
    printf("%ld\n", fibonacci(n));
    return 0;
}
