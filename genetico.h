#ifndef GENETICO_H
#define GENETICO_H

typedef struct {
    int *cromossomo;

    float custo_total;

    int *linhas_cobertas;
    int num_linhas_cobertas;

    int avaliacao;

} Individuo;

typedef struct {
    Solucao *individuos;
    int tamanho;

} Populacao;

Solucao gerar_individuo(Instancia *instancia);
void eliminar_redudancia(Solucao *solucao, Instancia *instancia);
Populacao gerar_populacao_inicial(int tamanho, Instancia *instancia);

#endif