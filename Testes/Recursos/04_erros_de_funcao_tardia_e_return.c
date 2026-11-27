// Teste - Erros de chamada adiada e de return ausente (item 12)
// Erros esperados (todos semanticos, o programa nao executa):
//   linha 14: 'sem_else' pode terminar sem return (if sem else)
//   linha 20: 'so_laco' pode terminar sem return (return so dentro do while)
//   linha 9:  'tardia' espera 2 argumentos (conferido no fim, pois e definida abaixo)
//   linha 10: 'nunca_definida' nao declarada (conferido no fim)

int main() {
    int a = tardia(1);
    int b = nunca_definida(2);
    return 0;
}

int sem_else(int x) {
    if (x > 0) {
        return 1;
    }
}

int so_laco(int x) {
    while (x > 0) {
        return x;
    }
}

int completa(int x) {
    if (x > 0) {
        return 1;
    } else {
        return 0;
    }
}

int tardia(int x, int y) {
    return x + y;
}
