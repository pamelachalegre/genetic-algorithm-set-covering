#include <stdlib.h>
#include <float.h>
#include "headers/auxiliares.h"

Solucao solucao_vazia(Instancia *instancia) {
    Solucao solucao;
    solucao.cromossomo = calloc(instancia->N, sizeof(int));
    solucao.linhas_cobertas = calloc(instancia->M, sizeof(int));
    solucao.num_colunas = 0;
    solucao.num_linhas = 0;
    solucao.custo_total = 0.0;
    solucao.avaliacao = 0.0;
    return solucao;
}

void adicionar_coluna(Solucao *solucao, int coluna, Instancia *instancia) {
    if (solucao->cromossomo[coluna]) return;

    solucao->cromossomo[coluna] = 1;
    solucao->custo_total += instancia->custo[coluna];
    solucao->num_colunas++;

    for (int indice_linha = 0; indice_linha < instancia->coluna_linhas_tam[coluna]; indice_linha++) {
        int linha = instancia->coluna_linhas[coluna][indice_linha];
        if (!solucao->linhas_cobertas[linha]) {
            solucao->linhas_cobertas[linha] = 1;
            solucao->num_linhas++;
        }
    }
}

void cobrir_linhas(Solucao *solucao, Instancia *instancia, int *linhas_descobertas, int *num_linhas_descobertas, int *permitidas) {
    while (*num_linhas_descobertas > 0) {
        // escolhe linha descoberta aleatoriamente
        int indice = rand() % (*num_linhas_descobertas);
        int linha = linhas_descobertas[indice];
        int melhor_coluna = -1;
        float melhor_pontuacao = FLT_MAX;

        // procura as colunas que cobrem a linha (candidatas) para encontrar a melhor
        for (int indice_coluna = 0; indice_coluna < instancia->linha_colunas_tam[linha]; indice_coluna++) {
            int coluna = instancia->linha_colunas[linha][indice_coluna];

            if (permitidas != NULL && !permitidas[coluna]) {
            } else { // contar linhas descobertas que essa coluna cobre
                int linhas_descobertas_cobertas = 0;
                for (int indice_descoberta = 0; indice_descoberta < *num_linhas_descobertas; indice_descoberta++) {
                    int encontrou = 0;
                    for (int indice_linha = 0; indice_linha < instancia->coluna_linhas_tam[coluna] && !encontrou; indice_linha++) {
                        if (instancia->coluna_linhas[coluna][indice_linha] == linhas_descobertas[indice_descoberta]) {
                            linhas_descobertas_cobertas++;
                            encontrou = 1;
                        }
                    }
                }
                if (linhas_descobertas_cobertas > 0) {
                    float pontuacao = instancia->custo[coluna] / (float)linhas_descobertas_cobertas;
                    if (pontuacao < melhor_pontuacao) {
                        melhor_pontuacao = pontuacao;
                        melhor_coluna = coluna;
                    }
                }
            }
        }
        if (melhor_coluna == -1) { // em instancias validas, toda linha possui pelo menos uma linha que a cobre
            linhas_descobertas[indice] = linhas_descobertas[*num_linhas_descobertas - 1];
            (*num_linhas_descobertas)--;
        } else {
            adicionar_coluna(solucao, melhor_coluna, instancia);

            for (int i = *num_linhas_descobertas - 1; i >= 0; i--) { // remove as linhas que a coluna inserida cobriu
                if (solucao->linhas_cobertas[linhas_descobertas[i]]) {
                    linhas_descobertas[i] = linhas_descobertas[*num_linhas_descobertas - 1];
                    (*num_linhas_descobertas)--;
                }
            }
        }
    }
}

void remover_coluna(Solucao *solucao, int coluna, Instancia *instancia) {
    if (!solucao->cromossomo[coluna]) return;
 
    solucao->cromossomo[coluna] = 0;
    solucao->custo_total -= instancia->custo[coluna];
    solucao->num_colunas--;
}
 
int coluna_cobre_linha(Instancia *instancia, int coluna, int linha) {
    for (int i = 0; i < instancia->coluna_linhas_tam[coluna]; i++) {
        if (instancia->coluna_linhas[coluna][i] == linha) {
            return 1;
        }
    }
    return 0;
}
 
int *construir_cobertura_por_linha(Solucao *solucao, Instancia *instancia) {
    int *cobertura_por_linha = calloc(instancia->M, sizeof(int)); 
 
    for (int coluna = 0; coluna < instancia->N; coluna++) {
        if (solucao->cromossomo[coluna]) {
            for (int i = 0; i < instancia->coluna_linhas_tam[coluna]; i++) {
                cobertura_por_linha[instancia->coluna_linhas[coluna][i]]++;
            }
        }
    }
 
    return cobertura_por_linha;
}