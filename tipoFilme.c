#include <stdio.h>
#include <stdlib.h>
#include "tipoFilme.h"


void ImprimeFilme(tFilme *filme){
    printf("Filme: %s [%.1f] (id=%d, votos=%d)\n", filme->titulo, filme->avaliacaoMedia, filme->id, filme->qVotos);
}
void LiberaFilme(tFilme *filme){
    free(filme->titulo);
}