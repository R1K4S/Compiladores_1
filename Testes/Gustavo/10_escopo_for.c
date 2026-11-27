int main() {
    /* 'i' do for nao deve ser visivel aqui fora */
    for (int i = 0; i < 3; i++) {
        printf("dentro: %d\n", i);
    }

    /* esta linha deve gerar erro semantico: i nao esta declarada aqui */
    printf("fora do for: %d\n", i);

    return 0;
}
