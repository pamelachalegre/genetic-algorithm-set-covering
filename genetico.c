#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <float.h>
#include "genetico.h"

Solucao gerar_individuo(Instancia *instancia) {
    int N = instancia->N;
    int M = instancia->M;

    Solucao solucao;
    solucao.cromossomo = calloc(N, sizeof(int));
    // solucao.conjunto_colunas = malloc(N * sizeof(int));
    solucao.linhas_cobertas = calloc(M, sizeof(int));
    solucao.num_colunas = 0;
    solucao.num_linhas = 0;
    solucao.custo_total = 0.0;
    solucao.avaliacao = 0.0;

    int *descobertas = malloc(M * sizeof(int));

    for (int i = 0; i < M; i++) {
        descobertas[i] = i;
    }

    int num_descobertas = M; // inicialmente, todas as linhas estao descobertas

    while (num_descobertas > 0) {
        // escolhe linha descoberta aleatoriamente
        int indice = rand() % num_descobertas;
        int linha = descobertas[indice];
        int num_candidatas = instancia->linha_colunas_tam[linha];

        if (num_candidatas > 0) { // remove linha sem coluna candidata
            int melhor_coluna = -1;
            float melhor_pontuacao = FLT_MAX;

            // procura as colunas que cobrem a linha (candidatas) para encontrar a melhor
            for (int indice_coluna = 0; indice_coluna < num_candidatas; indice_coluna++) {
                int coluna = instancia->linha_colunas[linha][indice_coluna];
                int novas_linhas = 0;

                for (int indice_linha = 0; indice_linha < instancia->coluna_linhas_tam[coluna]; indice_linha++) {
                    int linha_coberta = instancia->coluna_linhas[coluna][indice_linha];

                    if (!solucao.linhas_cobertas[linha_coberta]) {
                        novas_linhas++;
                    }
                }

                if (novas_linhas > 0) {
                    float pontuacao = instancia->custo[coluna] / (float)novas_linhas;
                    if (pontuacao < melhor_pontuacao) {
                        melhor_pontuacao = pontuacao;
                        melhor_coluna = coluna;
                    }
                }
            }

            // adicionar melhor coluna na solucao, se ainda nao estiver
            if (!solucao.cromossomo[melhor_coluna]) {
                solucao.cromossomo[melhor_coluna] = 1;
                solucao.custo_total += instancia->custo[melhor_coluna];
                solucao.num_colunas++;
            }

            // marcar todas as linhas que a melhor coluna cobre
            for (int indice_linha = 0; indice_linha < instancia->coluna_linhas_tam[melhor_coluna]; indice_linha++) {
                int linha_coberta = instancia->coluna_linhas[melhor_coluna][indice_linha];
                if (!solucao.linhas_cobertas[linha_coberta]) {
                    solucao.linhas_cobertas[linha_coberta] = 1;
                    solucao.num_linhas++;
                }
            }

            // atualizar lista de linhas descobertas
            for (int i = num_descobertas -1; i >= 0; i--) {
                if (solucao.linhas_cobertas[descobertas[i]]) {
                    descobertas[i] = descobertas[num_descobertas - 1];
                    num_descobertas--;
                }
            }
        } else { // linhas sem colunas candidatas
            descobertas[indice] = descobertas[num_descobertas -1];
            num_descobertas--;
        }
    }

    eliminar_redundancia(&solucao, instancia);
    free(descobertas);

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