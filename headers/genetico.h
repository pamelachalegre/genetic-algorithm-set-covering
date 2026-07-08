#ifndef GENETICO_H
#define GENETICO_H

#include "instancia.h"

#define TORNEIO 2
#define TAXA_MUTACAO 0.2
#define TAXA_ELITISMO 0.1
#define LAMBDA_MIN 0.15
#define LAMBDA_MAX 0.4
#define MAX_GERACOES 2000


typedef struct {
    float melhor_inicial;
    float melhor_final;
    int geracoes;
    int geracoes_sem_melhora;
    int geracao_ultima_melhoria;
    char *criterio_parada;
} Resultado;


/*
Elimina colunas redundantes para diminuição de custo.
*/
void eliminar_redundancia(Solucao *solucao, Instancia *instancia);

Solucao gerar_individuo(Instancia *instancia);

Populacao gerar_populacao_inicial(int tamanho, Instancia *instancia);

/*
Quantifica a qualidade genética dos cromossomos (qualidade de uma solução).
Percorre o cromossomo e soma o custo de cada coluna selecionada.
*/
void avaliar_individuo(Solucao *solucao, Instancia *instancia);

/*
Avalia a aptidão de todos os indivíduos da população.
*/
void avaliar_populacao(Populacao *populacao, Instancia *instancia);

/*
Determina quais indivíduos se reproduzirão para gerar a próxima população.
Utiliza técnica de torneio com k=TORNEIO candidatos aleatórios.
Escolhe o mais adaptado (menor avaliação).
*/
int selecao(Populacao *populacao);

/*
Executa uma combinação dos cromossomos pais para gerar novos cromossomos filhos.
Reconstrói o filho por heurística gulosa.
Utiliza apenas colunas herdadas de ao menos um dos pais.
*/
Solucao cruzamento(Solucao *pai1, Solucao *pai2, Instancia *instancia);

/*
Modifica uma parte do cromossomo para gerar diversidade.
Remove parte das colunas da solução.
Reconstrói as linhas descobertas pela heurística gulosa e elimina redundâncias.
*/
void mutacao(Solucao *solucao, Instancia *instancia);

/*
Utiliza elitismo: mantem parte da população anterior (os melhores indivíduos).
Substitui os não-elite pelos filhos gerados nessa geração.
*/
void atualizar_populacao(Populacao *populacao, Solucao *filhos, int num_filhos, Instancia *instancia);


Resultado algoritmo_genetico(int tamanho_populacao, int seed, int NMAX, Instancia *instancia);

#endif