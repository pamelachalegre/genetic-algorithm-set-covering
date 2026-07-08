#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "headers/instancia.h"
#include "headers/genetico.h"

int main(int argc, char *argv[]) {
    const char *arquivo = argv[1];

    Instancia *instancia = ler_instancia(arquivo);

    int tamanho_populacao = atoi(argv[2]);
    int seed = atoi(argv[3]);
    srand((unsigned int)seed);

    int MAX_SEM_MELHORA = instancia->N / 2;
    
    clock_t inicio = clock();

    Resultado ag = algoritmo_genetico(tamanho_populacao, seed, MAX_SEM_MELHORA, instancia);

    clock_t fim = clock();
    double tempo_execucao = (double)(fim - inicio) / CLOCKS_PER_SEC;

    printf("%d,%.2f,%.2f,%d,%d,%d,%s,%.6f\n", seed, ag.melhor_inicial, ag.melhor_final, ag.geracoes, ag.geracoes_sem_melhora, ag.geracao_ultima_melhoria, ag.criterio_parada, tempo_execucao);
    
    liberar_instancia(instancia);

    return 0;
}
