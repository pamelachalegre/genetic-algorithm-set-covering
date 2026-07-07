#include <stdlib.h>
#include "headers/buscalocal.h"
#include "headers/auxiliares.h"

/*
Busca local baseada em Yagiura et al. (2006) para o problema de cobertura de conjuntos.

1) 2-flip: troca uma coluna da solução por uma coluna fora dela (uma
posição vira 0, outra vira 1), desde que a troca reduza o custo e mantenha
todas as linhas cobertas.

2) 3-flip: troca duas colunas da solução por uma única coluna fora dela 
(duas posições viram 0, uma vira 1), desde que a coluna inserida cubra tudo 
que ficaria descoberto e custe menos que as duas removidas juntas.
*/

static int troca_mantem_cobertura(Instancia *instancia, int *cobertura_por_linha, int coluna1, int coluna2, int candidato) {
    // verifica as linhas que a primeira coluna removida cobre
    for (int i = 0; i < instancia->coluna_linhas_tam[coluna1]; i++) {
        int linha = instancia->coluna_linhas[coluna1][i];

        int quantas_cobrem = cobertura_por_linha[linha]; 

        quantas_cobrem = quantas_cobrem - 1; // tirando a coluna1, que cobre essa linha

        if (coluna2 != -1 && coluna_cobre_linha(instancia, coluna2, linha)) { 
            quantas_cobrem = quantas_cobrem - 1; // tirando a coluna2 também, se ela cobrir essa linha
        }

        if (coluna_cobre_linha(instancia, candidato, linha)) {
            quantas_cobrem = quantas_cobrem + 1; // o candidato entra e cobre essa linha
        }

        if (quantas_cobrem <= 0) {
            return 0; // essa linha ficaria descoberta, não pode trocar
        }
    }

    // se tiver uma segunda coluna saindo, verifica as linhas dela também
    if (coluna2 != -1) {
        for (int i = 0; i < instancia->coluna_linhas_tam[coluna2]; i++) {
            int linha = instancia->coluna_linhas[coluna2][i];

            int quantas_cobrem = cobertura_por_linha[linha];

            quantas_cobrem = quantas_cobrem - 1; // tirando a coluna2

            if (coluna_cobre_linha(instancia, coluna1, linha)) {
                quantas_cobrem = quantas_cobrem - 1; // tirando a coluna1 também, se ela cobrir essa linha
            }

            if (coluna_cobre_linha(instancia, candidato, linha)) {
                quantas_cobrem = quantas_cobrem + 1; // o candidato entra e cobre essa linha
            }

            if (quantas_cobrem <= 0) {
                return 0; 
            }
        }
    }

    return 1; // nenhuma linha ficou descoberta, pode trocar
}

static int doisflip(Solucao *solucao, Instancia *instancia, int *cobertura_por_linha) {
    // verifica colunas da solução
    for (int indice_coluna = 0; indice_coluna < instancia->N; indice_coluna++) {
        if (!solucao->cromossomo[indice_coluna]) continue;

        // verifica colunas fora da solução
        for (int indice_candidato = 0; indice_candidato < instancia->N; indice_candidato++) {
            if (solucao->cromossomo[indice_candidato]) continue;

            float custo_remocao = instancia->custo[indice_coluna];
            float custo_insercao = instancia->custo[indice_candidato];

            if (custo_insercao >= custo_remocao) continue;

            // confere se a troca deixa todas as linhas cobertas
            int pode_trocar = troca_mantem_cobertura(instancia, cobertura_por_linha, indice_coluna, -1, indice_candidato);

            if (pode_trocar) {
                remover_coluna(solucao, indice_coluna, instancia);
                adicionar_coluna(solucao, indice_candidato, instancia);
                return 1;
            }
        }
    }

    return 0;
}

static int tresflip(Solucao *solucao, Instancia *instancia, int *cobertura_por_linha) {
    // escolhe a primeira coluna da solução para remover
    for (int indice_coluna = 0; indice_coluna < instancia->N; indice_coluna++) {
        if (!solucao->cromossomo[indice_coluna]) continue;

        // escolhe a segunda coluna da solução para remover
        for (int indice_coluna2 = indice_coluna + 1; indice_coluna2 < instancia->N; indice_coluna2++) {
            if (!solucao->cromossomo[indice_coluna2]) continue;

            float custo_remocao = instancia->custo[indice_coluna] + instancia->custo[indice_coluna2];

            // verifica colunas fora da solução
            for (int indice_candidato = 0; indice_candidato < instancia->N; indice_candidato++) {
                if (solucao->cromossomo[indice_candidato]) continue;

                float custo_insercao = instancia->custo[indice_candidato];

                if (custo_insercao >= custo_remocao) continue;

                // confere se a troca deixa todas as linhas cobertas
                int pode_trocar = troca_mantem_cobertura(instancia, cobertura_por_linha, indice_coluna, indice_coluna2, indice_candidato);

                if (pode_trocar) {
                    remover_coluna(solucao, indice_coluna, instancia);
                    remover_coluna(solucao, indice_coluna2, instancia);
                    adicionar_coluna(solucao, indice_candidato, instancia);
                    return 1;
                }
            }
        }
    }

    return 0;
}

int busca_local(Solucao *solucao, Instancia *instancia) {
    int alguma_melhora = 0; // vira 1 se a busca local melhorar a solução pelo menos uma vez

    int *cobertura_por_linha = construir_cobertura_por_linha(solucao, instancia);

    int melhorou = 1;
    while (melhorou) {
        melhorou = 0;

        if (doisflip(solucao, instancia, cobertura_por_linha)) {
            melhorou = 1;
            alguma_melhora = 1;
        } else if (tresflip(solucao, instancia, cobertura_por_linha)) {
            melhorou = 1;
            alguma_melhora = 1;
        }

        if (melhorou) {
            // a solução mudou, então refaz a contagem de cobertura por linha do zero
            free(cobertura_por_linha);
            cobertura_por_linha = construir_cobertura_por_linha(solucao, instancia);
        }
    }

    free(cobertura_por_linha);

    solucao->avaliacao = solucao->custo_total;

    return alguma_melhora;
}