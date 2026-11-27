// Teste - Recuperacao de erros sintaticos (semana 05)
// O parser deve reportar TODOS os erros abaixo numa unica execucao,
// retomando a analise depois de ';' ou '}', e nao executar o programa.

int dobro(int v) {
    return v * ;          // erro 1 (linha 6): falta o operando
}

int main() {
    int x = ;             // erro 2 (linha 10): falta o valor
    int y = 2;
    {
        y = y + ;         // erro 3 (linha 13): dentro de um bloco
    }
    printf("%d\n", nao_declarada);   // erro 4 (linha 15): semantico, tambem e reportado
    return 0;
}
