#ifndef INSTANCIA_H
#define INSTANCIA_H

typedef struct{
    int M; // Número total de linhas (elementos a cobrir)
    int N; // Número total de colunas (subconjuntos)

    float *custo; // Custo de cada coluna

    // Para cada coluna, quais linhas ela cobre
    int **coluna_linhas;
    // Quantas linhas uma determinada coluna cobre
    int *coluna_linhas_tam; 

    // Para cada linha, quais colunas a cobrem
    int **linha_colunas;
    // Quantas colunas cobrem uma determinada linha
    int *linha_colunas_tam;
    
} Instancia;

#endif