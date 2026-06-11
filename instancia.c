#include <stdio.h>
#include <stdlib.h>
#include "instancia.h"

Instancia *ler_instancia(const char *arquivo) {
    FILE *file;
    file = fopen(arquivo, "r");

    Instancia *inst = malloc(sizeof(Instancia));

    // Lê valores de M (linhas) e N (colunas)

}