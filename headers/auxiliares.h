#ifndef AUXILIARES_H
#define AUXILIARES_H

#include "instancia.h"

Solucao solucao_vazia(Instancia *instancia);

void adicionar_coluna(Solucao *solucao, int coluna, Instancia *instancia);

void cobrir_linhas(Solucao *solucao, Instancia *instancia, int *linhas_descobertas, int *num_linhas_descobertas, int *permitidas);

void remover_coluna(Solucao *solucao, int coluna, Instancia *instancia);

int coluna_cobre_linha(Instancia *instancia , int coluna, int linha);

int *construir_cobertura_por_linha(Solucao *solucao, Instancia *instancia);

#endif