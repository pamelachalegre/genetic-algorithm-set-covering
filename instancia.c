#include <stdio.h>
#include <stdlib.h>
#include "instancia.h"

static void ler_cabecalho(FILE *file, Instancia *inst) {
    char linha[4096];  
    
    fgets(linha, sizeof(linha), file); 
    sscanf(linha, "LINHAS %d", &inst->M);

    fgets(linha, sizeof(linha), file);
    sscanf(linha, "COLUNAS %d", &inst->N);

    fgets(linha, sizeof(linha), file); // Descarta "DADOS"
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
}

//static void ler_colunas(FILE *file, Instancia *inst) 

//static void montar_linha_colunas(Instancia *inst)

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
    //ler_colunas(file, inst);
    //montar_linha_colunas(inst);
    
    fclose(file);
    return inst;
}