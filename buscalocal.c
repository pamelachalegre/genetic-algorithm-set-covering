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

        float custo_remocao = instancia->custo[indice_coluna];

        // acha as linhas que ficariam expostas ao remover essa coluna
        // (aquelas cobertas só por ela, cobertura_por_linha == 1) e, entre
        // elas, a que tem a menor lista de colunas candidatas (linha_critica)
        int linha_critica = -1;
        int existe_linha_exposta = 0;

        for (int i = 0; i < instancia->coluna_linhas_tam[indice_coluna]; i++) {
            int linha = instancia->coluna_linhas[indice_coluna][i];

            if (cobertura_por_linha[linha] == 1) { 
                existe_linha_exposta = 1;
                if (linha_critica == -1 || instancia->linha_colunas_tam[linha] < instancia->linha_colunas_tam[linha_critica]) { 
                    linha_critica = linha;
                }
            }
        }

        if (!existe_linha_exposta) {
            // nenhuma linha fica descoberta: qualquer coluna mais barata fora da solução resolve
            for (int indice_candidato = 0; indice_candidato < instancia->N; indice_candidato++) {
                if (solucao->cromossomo[indice_candidato]) continue;
                if (instancia->custo[indice_candidato] >= custo_remocao) continue;
                
                remover_coluna(solucao, indice_coluna, instancia);
                adicionar_coluna(solucao, indice_candidato, instancia);
                return 1;
            }
            continue;
        }

        // como existe linha exposta, o candidato precisa cobrir linha_critica,
        // entao so olha para as colunas que cobrem essa linha
        for (int idx = 0; idx < instancia->linha_colunas_tam[linha_critica]; idx++) {
            int indice_candidato = instancia->linha_colunas[linha_critica][idx];
            if (solucao->cromossomo[indice_candidato]) continue;

            float custo_insercao = instancia->custo[indice_candidato];
            if (custo_insercao >= custo_remocao) continue;

            // confere se a troca deixa todas as linhas cobertas (inclui as demais expostas, se houver)
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

            // acha as linhas que ficariam expostas ao remover as duas colunas
            // (cobertura_por_linha cai a zero descontando quem sai) e, entre
            // elas, a que tem a menor lista de colunas candidatas (linha_critica)
            int linha_critica = -1;
            int existe_linha_exposta = 0;

            for (int i = 0; i < instancia->coluna_linhas_tam[indice_coluna]; i++) {
                int linha = instancia->coluna_linhas[indice_coluna][i];
                int restante = cobertura_por_linha[linha] - 1; // tirando coluna1
                if (coluna_cobre_linha(instancia, indice_coluna2, linha)) restante--;

                if (restante <= 0) {
                    existe_linha_exposta = 1;
                    if (linha_critica == -1 || instancia->linha_colunas_tam[linha] < instancia->linha_colunas_tam[linha_critica]) {
                        linha_critica = linha;
                    }
                }
            }
            for (int i = 0; i < instancia->coluna_linhas_tam[indice_coluna2]; i++) {
                int linha = instancia->coluna_linhas[indice_coluna2][i];
                if (coluna_cobre_linha(instancia, indice_coluna, linha)) continue; // ja tratada acima

                int restante = cobertura_por_linha[linha] - 1; // tirando coluna2
                if (restante <= 0) {
                    existe_linha_exposta = 1;
                    if (linha_critica == -1 || instancia->linha_colunas_tam[linha] < instancia->linha_colunas_tam[linha_critica]) {
                        linha_critica = linha;
                    }
                }
            }

            if (!existe_linha_exposta) {
                // nenhuma linha fica descoberta: qualquer coluna mais barata fora da solução resolve
                for (int indice_candidato = 0; indice_candidato < instancia->N; indice_candidato++) {
                    if (solucao->cromossomo[indice_candidato]) continue;
                    if (instancia->custo[indice_candidato] >= custo_remocao) continue;

                    remover_coluna(solucao, indice_coluna, instancia);
                    remover_coluna(solucao, indice_coluna2, instancia);
                    adicionar_coluna(solucao, indice_candidato, instancia);
                    return 1;
                }
                continue;
            }

            // como existe linha exposta, o candidato precisa cobrir linha_critica,
            // entao so olha para as colunas que cobrem essa linha
            for (int idx = 0; idx < instancia->linha_colunas_tam[linha_critica]; idx++) {
                int indice_candidato = instancia->linha_colunas[linha_critica][idx];
                if (solucao->cromossomo[indice_candidato]) continue;

                float custo_insercao = instancia->custo[indice_candidato];
                if (custo_insercao >= custo_remocao) continue;

                // confere se a troca deixa todas as linhas cobertas (inclui as demais expostas, se houver)
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