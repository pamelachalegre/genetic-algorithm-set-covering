#ifndef INSTANCIA_H
#define INSTANCIA_H

typedef struct{
    int M; // Número total de linhas (elementos a cobrir)
    int N; // Número total de colunas (subconjuntos)

    float *custo; // Custo de cada coluna

    int **coluna_linhas; // Para cada coluna, quais linhas ela cobre
    int *coluna_linhas_tam; // Quantas linhas uma determinada coluna cobre

    int **linha_colunas; // Para cada linha, quais colunas a cobrem
    int *linha_colunas_tam; // Quantas colunas cobrem uma determinada linha

} Instancia;

/**
 * Solução candidata.
 */
typedef struct {
    int *cromossomo;
    int *linhas_cobertas;

    int num_linhas;
    int num_colunas;
    
    float custo_total;
    float avaliacao;

} Solucao;

Instancia *ler_instancia(const char *arquivo);
void liberar_instancia(Instancia *inst);

#endif