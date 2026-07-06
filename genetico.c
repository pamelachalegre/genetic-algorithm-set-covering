#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <float.h>
#include "headers/genetico.h"
#include "headers/auxiliares.h"


void eliminar_redundancia(Solucao *solucao, Instancia *instancia) {
    int N = instancia->N;
    int M = instancia->M;

    int *cobertura_por_linha = calloc(M, sizeof(int)); // quantas colunas cobrem cada linha

    for (int coluna = 0; coluna < N; coluna++) { // percorrer todas as linhas cobertas pela coluna atual
        if (solucao->cromossomo[coluna]) {
            for (int indice_linha = 0; indice_linha < instancia->coluna_linhas_tam[coluna]; indice_linha++) {
                cobertura_por_linha[instancia->coluna_linhas[coluna][indice_linha]]++;
            }
        }
    }
 
    // lista de colunas selecionadas ordenada por custo decrescente
    int *colunas_selecionadas = malloc(solucao->num_colunas * sizeof(int));
    int  qtde_colunas = 0;
    for (int coluna = 0; coluna < N; coluna++)
        if (solucao->cromossomo[coluna])
            colunas_selecionadas[qtde_colunas++] = coluna;
 
    // mais caro primeiro
    for (int i = 1; i < qtde_colunas; i++) {
        int coluna_atual = colunas_selecionadas[i];
        int j = i - 1;
        while (j >= 0 && instancia->custo[colunas_selecionadas[j]] < instancia->custo[coluna_atual]) {
            colunas_selecionadas[j + 1] = colunas_selecionadas[j];
            j--;
        }
        colunas_selecionadas[j + 1] = coluna_atual;
    }
    
    // tentar remover cada coluna por custo decrescente
    for (int indice_coluna = 0; indice_coluna < qtde_colunas; indice_coluna++) {
        int coluna = colunas_selecionadas[indice_coluna];
        int pode_remover = 1;

        for (int indice_linha = 0; indice_linha < instancia->coluna_linhas_tam[coluna]; indice_linha++) {
            int linha = instancia->coluna_linhas[coluna][indice_linha];
            
            if (cobertura_por_linha[linha] < 2) { // se tiver uma linha exclusiva, ja impede a remocao da coluna 
                pode_remover = 0;
            }
        }
 
        if (pode_remover) {
            solucao->cromossomo[coluna] = 0;
            solucao->custo_total -= instancia->custo[coluna];
            solucao->num_colunas--;
            for (int indice_linha = 0; indice_linha < instancia->coluna_linhas_tam[coluna]; indice_linha++)
                cobertura_por_linha[instancia->coluna_linhas[coluna][indice_linha]]--;
        }
    }
    free(colunas_selecionadas);
    free(cobertura_por_linha);
}


Solucao gerar_individuo(Instancia *instancia) {
    int M = instancia->M;

    Solucao solucao = solucao_vazia(instancia);

    int *linhas_descobertas = malloc(M * sizeof(int));
    int num_linhas_descobertas = M; // inicialmente, todas as linhas estao descobertas

    for (int i = 0; i < M; i++) {
        linhas_descobertas[i] = i;
    }

    cobrir_linhas(&solucao, instancia, linhas_descobertas, &num_linhas_descobertas, NULL);

    eliminar_redundancia(&solucao, instancia);

    free(linhas_descobertas);

    return solucao;
}


Populacao gerar_populacao_inicial(int tamanho, Instancia *instancia) {
    Populacao populacao;
    populacao.tamanho = tamanho;
    populacao.individuos = malloc(tamanho * sizeof(Solucao));

    for (int i = 0; i < tamanho; i++) {
        populacao.individuos[i] = gerar_individuo(instancia);
    }

    return populacao;
}


void avaliar_individuo(Solucao *solucao, Instancia *instancia) {
    float custo = 0.0;

    for (int coluna = 0; coluna < instancia->N; coluna++) {
        if (solucao->cromossomo[coluna]) {
            custo += instancia->custo[coluna];
        }
    }

    solucao->custo_total = custo;
    solucao->avaliacao = custo;
}


void avaliar_populacao(Populacao *populacao, Instancia *instancia) {
    for (int i = 0; i < populacao->tamanho; i++) {
        avaliar_individuo(&populacao->individuos[i], instancia);
    }
}


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


Solucao cruzamento(Solucao *pai1, Solucao *pai2, Instancia *instancia) {
    int N = instancia->N;
    int M = instancia->M;

    int *uniao_colunas = calloc(N, sizeof(int)); // vetor binário (uniao colunas dos pais)
    for (int coluna = 0; coluna < N; coluna++) {
        if (pai1->cromossomo[coluna] || pai2->cromossomo[coluna]) {
            uniao_colunas[coluna] = 1;
        }
    }

    Solucao filho = solucao_vazia(instancia);

    int *linhas_descobertas = malloc(M * sizeof(int));
    for (int i = 0; i < M; i++) {
        linhas_descobertas[i] = i;
    }
    int num_linhas_descobertas = M;

    cobrir_linhas(&filho, instancia, linhas_descobertas, &num_linhas_descobertas, uniao_colunas);

    eliminar_redundancia(&filho, instancia);
    filho.avaliacao = filho.custo_total;

    free(linhas_descobertas);
    free(uniao_colunas);
    return filho;
}


void mutacao(Solucao *solucao, Instancia *instancia) {
    int N = instancia->N;
    int M = instancia->M;

    // decidir se o individuo sofre mutacao
    float sorteio = rand() / RAND_MAX;
    if (sorteio >= TAXA_MUTACAO) return;

    // percentual de linhas a descobrir sorteado a cada mutacao (lambda pertence [0,1])
    float lambda = LAMBDA_MIN + (rand() / RAND_MAX) * (LAMBDA_MAX - LAMBDA_MIN);
    
    int meta_descobertas = lambda * M;
    if (meta_descobertas < 1) {
        meta_descobertas = 1;
    }

    // remove colunas até atingir meta_descobertas em linhas descobertas
    
    int linhas_descobertas_atual = 0;

    while (linhas_descobertas_atual < meta_descobertas && solucao->num_colunas > 0) {

        // escolher uma coluna ativa aleatoriamente
        int indice_sorteado = rand() % solucao->num_colunas;
        int coluna_remover = -1;

        // remove essa coluna e atualiza cobertura
        solucao->cromossomo[coluna_remover] = 0;
        solucao->custo_total -= instancia->custo[coluna_remover];
        solucao->num_colunas--;

    }

    // reconstruir linhas_cobertas 
    memset(solucao->linhas_cobertas, 0, M * sizeof(int));
    solucao->num_linhas = 0;

    // lista de linhas descobertas

    // cobrir as linhas descobertas com heurística gulosa
    int melhor_coluna = -1;
    float melhor_pontuacao = FLT_MAX;

    if (melhor_coluna == -1) { // se nao houver nenhuma candidata para essa linha, remove
    
    }

    // adiciona a coluna escolhida
    if (!solucao->cromossomo[melhor_coluna]) {
        solucao->cromossomo[melhor_coluna] = 1;
        solucao->custo_total += instancia->custo[melhor_coluna];
        solucao->num_colunas++;
    }
        // marca linhas cobertas e remover

        // remover as linhas que a coluna inserida cobriu

    eliminar_redundancia(solucao, instancia);
    solucao->avaliacao = solucao->custo_total;
}