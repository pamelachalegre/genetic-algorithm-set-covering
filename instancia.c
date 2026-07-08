#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "headers/instancia.h"
#include "headers/genetico.h"

static void ler_cabecalho(FILE *file, Instancia *inst) {
    char linha[4096];  
    
    fgets(linha, sizeof(linha), file); 
    sscanf(linha, "%*s %d", &inst->M);

    fgets(linha, sizeof(linha), file);
    sscanf(linha, "%*s %d", &inst->N);

    fgets(linha, sizeof(linha), file); // Descarta "DADOS" ou "Densidade"
}

static void alocar_arrays(Instancia *inst) {
    // Aloca array de custos (um float por coluna)
    inst->custo = malloc(inst->N * sizeof(float));

    // Aloca array de ponteiros: um int* por coluna (cada um vai apontar
    // para um array com os índices das linhas que essa coluna cobre)
    inst->coluna_linhas = malloc(inst->N * sizeof(int*)); 
    inst->coluna_linhas_tam = malloc(inst->N * sizeof(int)); 

    // Aloca array de ponteiros: um int* por linha 
    inst->linha_colunas = malloc(inst->M * sizeof(int*));
    inst->linha_colunas_tam = malloc(inst->M * sizeof(int));

    for (int i = 0; i < inst->M; i++) {
        inst->linha_colunas[i] = NULL;
        inst->linha_colunas_tam[i] = 0;
    }
}

static void ler_colunas(FILE *file, Instancia *inst) {
    char token[4096];

    while(fgets(token, sizeof(token), file)) {
        int id;
        float custo;

        char *ptr_token = strtok(token, " ");
        // Se o token for nulo, continua 
        if (ptr_token == NULL) {
            continue;
        }
        id = atoi(ptr_token);

        ptr_token = strtok(NULL, " ");
        if (ptr_token == NULL) {
            continue;
        }
        custo = atof(ptr_token);
        inst->custo[id - 1] = custo;

        int qte_linhas = 0;
        int *linhas = NULL;

        while((ptr_token = strtok(NULL, " ")) != NULL) { 
            qte_linhas++;
            linhas = realloc(linhas, qte_linhas * sizeof(int)); // Não se sabe quantas linhas cada coluna cobre -> realloc
            linhas[qte_linhas - 1] = atoi(ptr_token) - 1;  

            inst->linha_colunas_tam[linhas[qte_linhas - 1]]++;
        }
        inst->coluna_linhas_tam[id - 1] = qte_linhas;
        inst->coluna_linhas[id - 1] = linhas;
    }
}

static void montar_linha_colunas(Instancia *inst) {
    for (int i = 0; i < inst->M; i++) { 
        inst->linha_colunas[i] = malloc(inst->linha_colunas_tam[i] * sizeof(int));
    }
 
    // Cópia dos tamanhos para não alterar os tamanhos originais durante o preenchimento
    int *tam_restante = malloc(inst->M * sizeof(int));
    for (int i = 0; i < inst->M; i++) {
        tam_restante[i] = inst->linha_colunas_tam[i];
    }
 
    // Percorre as colunas
    for (int j = 0; j < inst->N; j++) {
        // Para cada linha que a coluna j cobre
        for (int i = 0; i < inst->coluna_linhas_tam[j]; i++) { 
            int linha = inst->coluna_linhas[j][i];    
            int linha_tam = tam_restante[linha];
            int *linha_colunas = inst->linha_colunas[linha];
            // Insere a coluna j no último espaço disponível da linha
            linha_colunas[linha_tam - 1] = j;
            tam_restante[linha] = linha_tam - 1;
        }
    }
 
    free(tam_restante);
}

Instancia *ler_instancia(const char *arquivo) {
    FILE *file;
    file = fopen(arquivo, "r");
    if (file == NULL) {
        printf("Erro ao abrir o arquivo!\n");
        return NULL;
    }

    Instancia *inst = malloc(sizeof(Instancia));

    ler_cabecalho(file, inst);
    alocar_arrays(inst);
    ler_colunas(file, inst);
    montar_linha_colunas(inst);
    
    fclose(file);
    return inst;
}

void liberar_instancia(Instancia *inst) {
    if (inst != NULL) {
        free(inst->custo);

        for (int i = 0; i < inst->N; i++) {
            free(inst->coluna_linhas[i]);
        }
        free(inst->coluna_linhas);
        free(inst->coluna_linhas_tam);

        for (int i = 0; i < inst->M; i++) {
            free(inst->linha_colunas[i]);
        }
        free(inst->linha_colunas);
        free(inst->linha_colunas_tam);

        free(inst);
    }
}

void liberar_solucao(Solucao *solucao) {
    free(solucao->cromossomo);
    free(solucao->linhas_cobertas);

    solucao->cromossomo = NULL;
    solucao->linhas_cobertas = NULL;
}

void liberar_populacao(Populacao *populacao) {
    for (int i = 0; i < populacao->tamanho; i++) {
        liberar_solucao(&populacao->individuos[i]);
    }

    free(populacao->individuos);
    populacao->individuos = NULL;
}