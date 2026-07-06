#ifndef AUXILIARES_H
#define AUXILIARES_H

#include "instancia.h"

Solucao solucao_vazia(Instancia *instancia);

void adicionar_coluna(Solucao *solucao, int coluna, Instancia *instancia);

void cobrir_linhas(Solucao *solucao, Instancia *instancia, int *linhas_descobertas, int *num_linhas_descobertas, int *permitidas);

#endif