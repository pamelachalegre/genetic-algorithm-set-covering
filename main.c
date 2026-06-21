#include <stdio.h>
#include "instancia.h"

int main() {
    Instancia *inst = ler_instancia("Teste_01.dat");
    
    if (inst == NULL) {
        printf("Erro ao ler a instância.\n");
        return 1;
    }

    printf("M=%d N=%d\n", inst->M, inst->N);
    printf("Coluna 1: custo=%.2f, cobre %d linhas\n", inst->custo[0], inst->coluna_linhas_tam[0]);

    return 0;
}