#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "tabelaIndexada.h"
#include "tabelaEncadeada.h"

#define MAX_LINHA 5000

void RemoveQuebraLinha(char *str){
    if (str[strlen(str) - 1] == '\n')
        str[strlen(str) - 1] = '\0';
}

int CarregaArquivo(const char *arq, tTabelaIdx *tabIdx, tTabelaEnc **tabEnc){
    FILE *fp;
    char linha[MAX_LINHA+1], *id, *titulo, *avaliacao, *votos, *virg, *aspas;
    int cont;
    tFilme filme;

    fp = fopen(arq, "r");
    if (!fp){
        puts("Erro ao abrir arquivo.");
        exit(1);
    }

    cont = 0;
    while(1){
        fgets(linha, MAX_LINHA+1, fp);
        if (feof(fp)){
            break;
        }
        RemoveQuebraLinha(linha);
        virg = strchr(linha, ',');
        if (virg){
            virg[-1] = '\0';
            id = linha+1;

            titulo = virg + 2;
            aspas = strchr(titulo, '\"');
            *aspas = '\0';
            filme.id = atoi(id);
            filme.titulo = malloc(sizeof(char) * strlen(titulo) + 1);
            if (!filme.titulo){
                printf("Erro ao alocar titulo do filme #%d\n", cont);
            }
            strcpy(filme.titulo, titulo);

            avaliacao = aspas + 3;
            aspas = strchr(avaliacao, '\"');
            *aspas = '\0';
            filme.avaliacaoMedia = strtod(avaliacao, NULL);

            votos = aspas + 3;
            aspas = strchr(votos, '\"');
            *aspas = '\0';
            filme.qVotos = strtod(votos, NULL);

            cont++;
            //printf("Filme: %s [%s] (id=%s, votos=%s)\n", titulo, avaliacao, id, votos);
            AcrescentaElementoIdx(tabIdx, &filme);
            AcrescentaElementoEnc(tabEnc, &filme);
        }
    }

    fclose(fp);
    return cont;
}

int BuscaComMovParaInicioIdx(tTabelaIdx *tabIdx, int id) {
    for (int i = 0; i < tabIdx->tamanho; i++) {
        if (tabIdx->filmes[i].id == id) {
            // movendo o elemento pro inicio
            tFilme encontrado = tabIdx->filmes[i];
            for (int j = i; j > 0; j--) {
                tabIdx->filmes[j] = tabIdx->filmes[j - 1];
            }
            tabIdx->filmes[0] = encontrado;

            return 0; // retorna indice 0 pois o elemento agora esta no inicio
        }
    }
    return -1; // elemento nao encontrado
}

tFilme* BuscaComMovParaInicioEnc(tTabelaEnc *tabEnc, int id) {
    tNo *anterior = NULL, *atual = tabEnc->inicio;

    while (atual) {
        if (atual->filme.id == id) {
            // se ja estiver no inicio, retorna diretamente
            if (anterior == NULL) {
                return &atual->filme;
            }

            // movendo o no para o inicio
            anterior->prox = atual->prox;
            atual->prox = tabEnc->inicio;
            tabEnc->inicio = atual;

            return &atual->filme;
        }

        anterior = atual;
        atual = atual->prox;
    }
    return NULL; // elemento nao encontrado
}

int BuscaComTransposicaoIdx(tTabelaIdx *tabIdx, int id) {
    for (int i = 0; i < tabIdx->tamanho; i++) {
        if (tabIdx->filmes[i].id == id) {
            // Se não for o primeiro elemento, realiza a transposição
            if (i > 0) {
                tFilme temp = tabIdx->filmes[i];
                tabIdx->filmes[i] = tabIdx->filmes[i - 1];
                tabIdx->filmes[i - 1] = temp;

                return i - 1; // Retorna o novo índice do elemento
            }
            return i; // Retorna o índice original se já está no início
        }
    }
    return -1; // Elemento não encontrado
}

tFilme* BuscaComTransposicaoEnc(tTabelaEnc *tabEnc, int id) {
    tNo *anterior = NULL, *preAnterior = NULL, *atual = tabEnc->inicio;

    while (atual) {
        if (atual->filme.id == id) {
            // Se já estiver no início, não faz transposição
            if (anterior == NULL) {
                return &atual->filme;
            }

            // Transposição: troca o nó atual com o anterior
            if (preAnterior != NULL) {
                preAnterior->prox = atual;
            } else {
                tabEnc->inicio = atual;
            }
            anterior->prox = atual->prox;
            atual->prox = anterior;

            return &atual->filme;
        }

        preAnterior = anterior;
        anterior = atual;
        atual = atual->prox;
    }
    return NULL; // Elemento não encontrado
}


int main(void){
    tTabelaIdx *tabIdx;
    tTabelaEnc *tabEnc;
    int q, mat;
    clock_t inicio, fim;
    double tempoDeCpu;

    tabIdx = CriaTabelaIdx();
    CriaTabelaEnc(&tabEnc);
    inicio = clock();
    q = CarregaArquivo("../../TMDB_movie_dataset_v11.csv", tabIdx, &tabEnc);
    fim = clock();
    tempoDeCpu = ((double) (fim - inicio)) / CLOCKS_PER_SEC;

    printf("%d filmes carregados.\n", q);
    printf("Tamanho tabela indexada: %d\n", ComprimentoIdx(tabIdx));
    printf("Tamanho tabela encadeada: %d\n", ComprimentoEnc(tabEnc));
    printf("Tempo de CPU: %lf\n", tempoDeCpu);


   while(1){
        printf("\nID: ");
        scanf("%d", &mat);

        inicio = clock();
        int ind = BuscaSequencial(tabIdx, mat);
        fim = clock();
        tempoDeCpu = ((double) (fim - inicio)) / CLOCKS_PER_SEC;
        printf("Operacoes busca sequencial: %d\n", QuantOperacoes());
        printf("Tempo de CPU: %lf\n", tempoDeCpu);
        if (ind < 0){
            puts("filme nao encontrado");
        }else{
            tFilme filme = ObtemElementoIdx(tabIdx, ind);
            ImprimeFilme(&filme);
        }

        inicio = clock();
        tFilme * filme = BuscaSequencialEnc(tabEnc, mat);
        fim = clock();
        tempoDeCpu = ((double) (fim - inicio)) / CLOCKS_PER_SEC;
        printf("Operacoes busca encadeada: %d\n", QuantOperacoesEnc());
        printf("Tempo de CPU: %lf\n", tempoDeCpu);
        if (!filme){
            puts("filme nao encontrado");
        }else{
            ImprimeFilme(filme);
        }

        // Busca com movimentação para o início na tabela indexada
        inicio = clock();
        int ind = BuscaComMovParaInicioIdx(tabIdx, mat);
        fim = clock();
        tempoDeCpu = ((double) (fim - inicio)) / CLOCKS_PER_SEC;
        printf("Tempo de CPU (Idx - Mov. para Início): %lf\n", tempoDeCpu);
        if (ind < 0) {
            puts("Filme não encontrado");
        } else {
            tFilme filme = tabIdx->filmes[0]; // Elemento sempre estará no início
            ImprimeFilme(&filme);
        }

        // Busca com movimentação para o início na tabela encadeada
        inicio = clock();
        tFilme *filmeEnc = BuscaComMovParaInicioEnc(tabEnc, mat);
        fim = clock();
        tempoDeCpu = ((double) (fim - inicio)) / CLOCKS_PER_SEC;
        printf("Tempo de CPU (Enc - Mov. para Início): %lf\n", tempoDeCpu);
        if (!filmeEnc) {
            puts("Filme não encontrado");
        } else {
            ImprimeFilme(filmeEnc);
        }

        // Busca com transposição na tabela indexada
        inicio = clock();
        int ind = BuscaComTransposicaoIdx(tabIdx, mat);
        fim = clock();
        tempoDeCpu = ((double) (fim - inicio)) / CLOCKS_PER_SEC;
        printf("Tempo de CPU (Idx - Transposição): %lf\n", tempoDeCpu);
        if (ind < 0) {
            puts("Filme não encontrado");
        } else {
            tFilme filme = tabIdx->filmes[ind];
            ImprimeFilme(&filme);
        }

        // Busca com transposição na tabela encadeada
        inicio = clock();
        tFilme *filmeEnc = BuscaComTransposicaoEnc(tabEnc, mat);
        fim = clock();
        tempoDeCpu = ((double) (fim - inicio)) / CLOCKS_PER_SEC;
        printf("Tempo de CPU (Enc - Transposição): %lf\n", tempoDeCpu);
        if (!filmeEnc) {
            puts("Filme não encontrado");
        } else {
            ImprimeFilme(filmeEnc);
        }


   }

    //DestroiTabelaEnc(tabEnc);
    DestroiTabelaIdx(tabIdx);
    return 0;
}
