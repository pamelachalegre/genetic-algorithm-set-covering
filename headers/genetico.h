#ifndef GENETICO_H
#define GENETICO_H

#include "instancia.h"

#define TORNEIO 2
#define TAXA_MUTACAO 0.02

typedef struct {
    Solucao *individuos;
    int tamanho;
} Populacao;

// Geração e avaliação
void eliminar_redundancia(Solucao *solucao, Instancia *instancia);
Solucao gerar_individuo(Instancia *instancia);
Populacao gerar_populacao_inicial(int tamanho, Instancia *instancia);

// Operadores genéticos
void avaliar_individuo(Solucao *solucao, Instancia *instancia);
void avaliar_populacao(Populacao *populacao, Instancia *instancia);
int selecao(Populacao *populacao);
Solucao cruzamento(Solucao *pai1, Solucao *pai2, Instancia *instancia);
void mutacao(Solucao *solucao, Instancia *instancia);

#endif