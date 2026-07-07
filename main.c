#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "headers/instancia.h"
#include "headers/genetico.h"

int main(int argc, char *argv[]) {
    srand(time(NULL)); // inicializa gerador de números aleatórios

    const char *arquivo = argv[1];
    Instancia *instancia = ler_instancia(arquivo);
    int tamanho_populacao = atoi(argv[2]);
    int NMAX = tamanho_populacao * 2;
    
    // Solucao melhor = algoritmo_genetico(tamanho_populacao, NMAX, *instancia)
    algoritmo_genetico(tamanho_populacao, NMAX, instancia);
    
    // liberar_instancia(&melhor);

    return 0;
}