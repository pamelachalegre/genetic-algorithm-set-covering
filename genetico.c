#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "genetico.h"

Solucao gerar_individuo(Instancia *instancia) {

}

void eliminar_redudancia(Solucao *solucao, Instancia *instancia) {

}

Populacao gerar_populacao_inicial(int tamanho, Instancia *instancia) {
    Populacao populacao;
    populacao.tamanho = tamanho;
    populacao.individuos = malloc(tamanho * sizeof(Solucao));

    for(int i = 0; i < tamanho; i++) {
        populacao.individuos[i] = gerar_individuo(instancia);
        eliminar_redundancia(&populacao.individuos[i], instancia);
    }

    return populacao;
}