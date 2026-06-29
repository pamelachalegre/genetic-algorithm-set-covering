#ifndef GENETICO_H
#define GENETICO_H

#include "instancia.h"

#define TORNEIO 2

typedef struct {
    Solucao *individuos;
    int tamanho;
} Populacao;

// Geração e avaliação
Solucao gerar_individuo(Instancia *instancia);
void eliminar_redundancia(Solucao *solucao, Instancia *instancia);
Populacao gerar_populacao_inicial(int tamanho, Instancia *instancia);

// Operadores genéticos
int selecao(Populacao *populacao);

#endif