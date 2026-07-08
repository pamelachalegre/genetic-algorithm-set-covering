#define VIA 3 // É o padrão, mas pode ser alterado para 1, 2 ou 3 para testar as vias de execução do algoritmo genético

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <float.h>
#include "headers/genetico.h"
#include "headers/auxiliares.h"
#include "headers/instancia.h"
#include "headers/buscalocal.h"


void eliminar_redundancia(Solucao *solucao, Instancia *instancia) {
    if (VIA == 1) return; // Se for a Via 1, ignora a limpeza e sai da função

    int N = instancia->N;
    // int M = instancia->M;

    int *cobertura_por_linha = construir_cobertura_por_linha(solucao, instancia);
 
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
            remover_coluna(solucao, coluna, instancia);
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

    for (int k = 1; k < TORNEIO; k++){
        int candidato = rand() % populacao->tamanho;

        if (populacao->individuos[candidato].avaliacao < populacao->individuos[melhor].avaliacao) {
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
    float sorteio = (float)rand() / RAND_MAX;
    if (sorteio >= TAXA_MUTACAO) return;

    // percentual de linhas a descobrir sorteado a cada mutacao (lambda pertence [0,1])
    float lambda = LAMBDA_MIN + ((float)rand() / RAND_MAX) * (LAMBDA_MAX - LAMBDA_MIN);
    int meta_descobertas = (int)(lambda * M);
    if (meta_descobertas < 1) {
        meta_descobertas = 1;
    }

    // remove colunas até atingir meta_descobertas em linhas descobertas
    int *cobertura_por_linha = calloc(M, sizeof(int));
    for (int coluna = 0; coluna < N; coluna++) {
        if (solucao->cromossomo[coluna]) {
            for (int indice_linha = 0; indice_linha < instancia->coluna_linhas_tam[coluna]; indice_linha++) {
                cobertura_por_linha[instancia->coluna_linhas[coluna][indice_linha]]++;
            }
        }
    }

    int linhas_descobertas_atual = 0;
    while (linhas_descobertas_atual < meta_descobertas && solucao->num_colunas > 0) {
        int indice_sorteado = rand() % solucao->num_colunas; // escolher uma coluna ativa aleatoriamente
        int coluna_remover = -1;
        int contagem = 0;
        for (int coluna = 0; coluna < N; coluna++) {
            if (!solucao->cromossomo[coluna]) { continue; }
            if (contagem == indice_sorteado) {
                coluna_remover = coluna;
                break;
            }
            contagem++;
        }
        if (coluna_remover == -1) { break; }

        remover_coluna(solucao, coluna_remover, instancia); // atualiza cobertura

        for (int indice_linha = 0; indice_linha < instancia->coluna_linhas_tam[coluna_remover]; indice_linha++) {
            int linha = instancia->coluna_linhas[coluna_remover][indice_linha];
            cobertura_por_linha[linha]--;
            if (cobertura_por_linha[linha] == 0)
                linhas_descobertas_atual++;
        }
    }
    free(cobertura_por_linha);

    // reconstruir linhas_cobertas 
    memset(solucao->linhas_cobertas, 0, M * sizeof(int));
    solucao->num_linhas = 0;
    for (int coluna = 0; coluna < N; coluna++) {
        if (!solucao->cromossomo[coluna]) { continue; }
        for (int indice_linha = 0; indice_linha < instancia->coluna_linhas_tam[coluna]; indice_linha++) {
            int linha = instancia->coluna_linhas[coluna][indice_linha];
            if (!solucao->linhas_cobertas[linha]) {
                solucao->linhas_cobertas[linha] = 1;
                solucao->num_linhas++;
            }
        }
    }
    int *linhas_descobertas = malloc(M * sizeof(int));
    int num_linhas_descobertas = 0;
    for (int linha = 0; linha < M; linha++)
        if (!solucao->linhas_cobertas[linha])
            linhas_descobertas[num_linhas_descobertas++] = linha;

    cobrir_linhas(solucao, instancia, linhas_descobertas, &num_linhas_descobertas, NULL);

    eliminar_redundancia(solucao, instancia);
    free(linhas_descobertas);
    solucao->avaliacao = solucao->custo_total;
}


void atualizar_populacao(Populacao *populacao, Solucao *filhos, int num_filhos, Instancia *instancia) {
    int tamanho_populacao = populacao->tamanho;
    int num_elite = (int)(tamanho_populacao * TAXA_ELITISMO);

    if (num_elite < 1) {
        num_elite = 1;
    }
    if (num_elite >= tamanho_populacao)  {
        num_elite = tamanho_populacao - 1;
    }
    int *indices_elite = malloc(num_elite * sizeof(int)); // identificar indices dos melhores individuos
    for (int i = 0; i < num_elite; i++) {
        indices_elite[i] = i;
    }
    for (int i = num_elite; i < tamanho_populacao; i++) { // busca pelo pior na elite atual
        int indice_pior_elite = 0;
        for (int k = 1; k < num_elite; k++) {
            if (populacao->individuos[indices_elite[k]].avaliacao > populacao->individuos[indices_elite[indice_pior_elite]].avaliacao)
                indice_pior_elite = k;
        }
        if (populacao->individuos[i].avaliacao < populacao->individuos[indices_elite[indice_pior_elite]].avaliacao) { // encontra melhor que o pior da elite, entra na elite
            indices_elite[indice_pior_elite] = i;
        }
    }
    int *individuo_elite = calloc(tamanho_populacao, sizeof(int));
    for (int k = 0; k < num_elite; k++) {
        individuo_elite[indices_elite[k]] = 1;
    }
    for (int i = 1; i < num_filhos; i++) { // ordena filhos para entrar na população - melhor primeiro
        Solucao filho_atual = filhos[i]; 
        int j = i - 1;
        while (j >= 0 && filhos[j].avaliacao > filho_atual.avaliacao) {
            filhos[j + 1] = filhos[j];
            j--;
        }
        filhos[j + 1] = filho_atual;
    }
    int filho_atual = 0;
    for (int i = 0; i < tamanho_populacao && filho_atual < num_filhos; i++) { // substituir nao-elite pelos filhos
        if (!individuo_elite[i]) {
            liberar_solucao(&populacao->individuos[i]);
            populacao->individuos[i] = filhos[filho_atual];
            filhos[filho_atual].cromossomo = NULL;
            filhos[filho_atual].linhas_cobertas = NULL;

            filho_atual++;
        }
    }
    for (int i = filho_atual; i < num_filhos; i++) { // libera filhos que nao entraram na populacao
        if (filhos[i].cromossomo != NULL) {
            liberar_solucao(&filhos[i]);
        }
    }
    free(indices_elite);
    free(individuo_elite);
}


Resultado algoritmo_genetico(int tamanho_populacao, int seed, int MAX_SEM_MELHORA, Instancia *instancia) {
    Populacao populacao = gerar_populacao_inicial(tamanho_populacao, instancia);
    avaliar_populacao(&populacao, instancia);

    float melhor_inicial = FLT_MAX;

    for (int i = 0; i < populacao.tamanho; i++) { // encontra o melhor da população inicial
        if (populacao.individuos[i].avaliacao < melhor_inicial) {
            melhor_inicial = populacao.individuos[i].avaliacao;
        }
    }
    
    float melhor_custo_global = melhor_inicial;
    int geracao_atual = 0;
    int geracoes_sem_melhora = 0;
    int geracao_ultima_melhoria = -1; // nenhuma melhoria encontrada

    //Geração de gráfico de convergência:
    //FILE *f_conv = fopen("resultados/convergencia_wren01.csv", "w"); 
    //fprintf(f_conv, "Geracao,Custo\n");

    while (geracoes_sem_melhora < MAX_SEM_MELHORA && geracao_atual < MAX_GERACOES) {        
        // selecao dos pais para cruzar os reprodutores
        int pai1 = selecao(&populacao);
        int pai2 = selecao(&populacao);

        while (pai1 == pai2) { // garantir pais diferentes
            pai2 = selecao(&populacao);
        }

        Solucao filho = cruzamento(&populacao.individuos[pai1], &populacao.individuos[pai2], instancia);

        mutacao(&filho, instancia);

        avaliar_individuo(&filho, instancia);

        if (VIA == 3) {
            busca_local(&filho, instancia); // Faz busca local se for a Via 3
        }

        atualizar_populacao(&populacao, &filho, 1, instancia);

        if (filho.avaliacao < melhor_custo_global - 0.001f) {
            melhor_custo_global = filho.avaliacao;
            geracoes_sem_melhora = 0;
            geracao_ultima_melhoria = geracao_atual;
        } else {
            geracoes_sem_melhora++;
        }

        //fprintf(f_conv, "%d,%.2f\n", geracao_atual, melhor_custo_global);

        geracao_atual++;
    }
    //fclose(f_conv);

    Resultado resultado;
    resultado.melhor_inicial = melhor_inicial;
    resultado.melhor_final = melhor_custo_global;
    resultado.geracoes = geracao_atual;
    resultado.geracoes_sem_melhora = geracoes_sem_melhora;
    resultado.geracao_ultima_melhoria = geracao_ultima_melhoria;

    if (geracoes_sem_melhora >= MAX_SEM_MELHORA) {
        resultado.criterio_parada = "estagnacao";
    } else {
        resultado.criterio_parada = "max_geracoes";
    }

    liberar_populacao(&populacao);

    return resultado;
}
