#include <stdio.h>
#include <stdlib.h>
#include "instancia.h"

Instancia *ler_instancia(const char *arquivo) {
    FILE *file;
    file = fopen(arquivo, "r");
    if (file == NULL) {
        printf("Erro ao abrir o arquivo!\n");
        return NULL;
    }
    Instancia *inst = malloc(sizeof(Instancia));

    // Lê valores de M (linhas) e N (colunas)
    char linha[4096];  
    
    fgets(linha, sizeof(linha), file); 
    sscanf(linha, "LINHAS %d", &inst->M);

    fgets(linha, sizeof(linha), file);
    sscanf(linha, "COLUNAS %d", &inst->N);

    fgets(linha, sizeof(linha), file); // Lê a linha dos dados e pula

    // Lê os custos das colunas 
    float custo;
    inst->custo = malloc(inst->N * sizeof(float));

    // Lê quais linhas cada coluna cobre
    inst->coluna_linhas = malloc(inst->N * sizeof(int*)); 
    inst->coluna_linhas_tam = malloc(inst->N * sizeof(int)); 

    // Lê para cada linha, quais colunas a cobrem
    inst->linha_colunas = malloc(inst->M * sizeof(int*));
    inst->linha_colunas_tam = malloc(inst->M * sizeof(int));

    return inst;
}