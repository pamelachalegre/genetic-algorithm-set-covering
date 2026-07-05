#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <float.h>
#include "headers/genetico.h"

/*
Elimina colunas redundantes para diminuição de custo.
*/
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
Quantifica a qualidade genética dos cromossomos (qualidade de uma solução).
Percorre o cromossomo e soma o custo de cada coluna selecionada.
*/
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

/*
Avalia a aptidão de todos os indivíduos da população.
*/
void avaliar_populacao(Populacao *populacao, Instancia *instancia) {
    for (int i = 0; i < populacao->tamanho; i++) {
        avaliar_individuo(&populacao->individuos[i], instancia);
    }
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

Solucao cruzamento(Solucao *pai1, Solucao *pai2, Instancia *instancia) {
    int N = instancia->N;
    int M = instancia->M;

    int *uniao_colunas = calloc(N, sizeof(int));
    for (int coluna = 0; coluna < N; coluna++) {
        if (pai1->cromossomo[coluna] || pai2->cromossomo[coluna]) {
            uniao_colunas[coluna] = 1;
        }
    }

    Solucao filho;
    filho.cromossomo = calloc(N, sizeof(int));
    filho.linhas_cobertas = calloc(M, sizeof(int));
    filho.num_colunas = 0;
    filho.num_linhas = 0;
    filho.custo_total = 0.0;
    filho.avaliacao = 0.0;

    int *descobertas = malloc(M * sizeof(int));
    for (int i = 0; i < M; i++)
        descobertas[i] = i;
    int num_descobertas = M;

    while (num_descobertas > 0) {
        int indice = rand() % num_descobertas;
        int linha = descobertas[indice];

        // lista de candidatas restrita a uniao_colunas
        int melhor_coluna = -1;
        float melhor_pontuacao = FLT_MAX;

        // restringir o cruzamento para considerar só colunas dos pais

        if (!filho.cromossomo[melhor_coluna]) {
            filho.cromossomo[melhor_coluna] = 1;
            filho.custo_total += instancia->custo[melhor_coluna];
            filho.num_colunas++;
        }

        for (int indice_linha = 0; indice_linha < instancia->coluna_linhas_tam[melhor_coluna]; indice_linha++) {
            int linha_coberta = instancia->coluna_linhas[melhor_coluna][indice_linha];
            if (!filho.linhas_cobertas[linha_coberta]) {
                filho.linhas_cobertas[linha_coberta] = 1;
                filho.num_linhas++;
            }
        }
    }

    eliminar_redundancia(&filho, instancia);
    filho.avaliacao = filho.custo_total;

    free(descobertas);
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