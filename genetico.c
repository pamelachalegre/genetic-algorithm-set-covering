#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "genetico.h"

Solucao gerar_individuo(Instancia *instancia) {
    int N = instancia->N;
    int M = instancia->M;

    Solucao solucao;
    solucao.cromossomo = calloc(N, sizeof(int));
    solucao.conjunto_colunas = malloc(N * sizeof(int));
    solucao.num_colunas = 0;
    solucao.linhas_cobertas = calloc(M, sizeof(int));
    solucao.num_linhas = 0;
    solucao.custo_total = 0.0;
    solucao.avaliacao = 0.0;

    int *descobertas = malloc(M * sizeof(int));

    for (int i = 0; i < M; i++) {
        descobertas[i] = i;
    }

    int num_descobertas = M;

    while (num_descobertas > 0) {
        int linha;
        int num_candidatas = inst->linha_colunas_tam[linha]

        if (num_candidatas == 0) { // linha impossível

        }


        // atualizar lista de linhas descobertas
    }

    eliminar_redudancia(&solucao, instancia);

    return solucao;
}

void eliminar_redudancia(Solucao *solucao, Instancia *instancia) {

}

Populacao gerar_populacao_inicial(int tamanho, Instancia *instancia) {
    Populacao populacao;
    populacao.tamanho = tamanho;
    populacao.individuos = malloc(tamanho * sizeof(Solucao));

    for(int i = 0; i < tamanho; i++) {
        populacao.individuos[i] = gerar_individuo(instancia);
    }

    return populacao;
}


/*
Determina quais indivíduos se reproduzirão para gerar a próxima população.
Utiliza técnica de torneio com k=TORNEIO candidatos aleatórios.
Escolhe o mais adaptado (menor avaliação).
*/
int selecao(Populacao *populacao) {
    int melhor = rand() % populacao->tamanho;

    for(int k = 1; k < TORNEIO; k++){
        int candidato = rand() % populacao->tamanho;

        if(populacao->individuos[candidato].avaliacao < populacao->individuos[melhor].avaliacao) {
            melhor = candidato;
        }
    }

    return melhor;
}