//
//  main.c
//  PCTSP
//
//  Created by Válber Laux on 12/17/15.
//  Copyright (c) 2015 Válber Laux. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<sys/timeb.h>

#define INF 9999999
#define PRESENTE 1
#define ALPHA 0.3

void inicia_funcaoaleatoria(void)
{
	struct timeb tmb;
	ftime(&tmb);
	srand(tmb.time*1000 + tmb.millitm);
}

void le_premios(int* premios, int n_vertices){
	int i;
	FILE *instancia;
	instancia = fopen("/Users/valberlaux/PCTSP/PCTSP/instancias/w10.txt", "r");
	
	if (instancia == NULL){
		printf("Erro ao carregar instância\n");
		exit(EXIT_FAILURE);
	}
	
	for (i=0; i<n_vertices; i++) {
		fscanf(instancia, "%d", &premios[i]);
	}
	
	fclose(instancia);
	
}

void le_penalidades(int* penalidades, int n_vertices){
	int i;
	FILE* instancia = fopen("/Users/valberlaux/PCTSP/PCTSP/instancias/p10.txt", "r");
	
	if (instancia == NULL){
		printf("Erro ao carregar instância\n");
		exit(EXIT_FAILURE);
	}
	
	for (i=0; i<n_vertices; i++) {
		fscanf(instancia, "%d", &penalidades[i]);
	}
	
	fclose(instancia);
}

void le_matriz_distancias(int** matriz_distancias, int n_vertices){
	int i, j;
	
	FILE* instancia = fopen("/Users/valberlaux/PCTSP/PCTSP/instancias/d10.txt", "r");
	
	if (instancia == NULL){
		printf("Erro ao carregar instância\n");
		exit(EXIT_FAILURE);
	}
	
	for (i=0; i<n_vertices; i++) {
		for (j=0; j<n_vertices; j++) {
			fscanf(instancia, "%d", &matriz_distancias[i][j]);
		}
	}
	
	fclose(instancia);
}

int* zeros(int* vetor, int tamanho){
	int i;
	if (vetor == NULL) {
		vetor = malloc(tamanho*sizeof(int));
	}
	for (i=0; i<tamanho; i++) {
		vetor[i] = 0;
	}
	return vetor;
}

void adiciona_vertice_solucao(int *solucao, int* vertices_na_solucao, int *tamanho_solucao, int vertice){
	solucao[*tamanho_solucao] = vertice;
	vertices_na_solucao[vertice] = 1;
	*tamanho_solucao = *tamanho_solucao + 1;
}

int calcula_custo_solucao(int* solucao, int tamanho_solucao, int n_vertices, int* premios, int* penalidades, int** matriz_distancias){
	int* vertices_na_solucao = NULL;
	vertices_na_solucao = zeros(vertices_na_solucao, n_vertices);
	int custo = 0;
	int i;
	//somatório dos prêmios dos vértices visitados
	for (i=0; i<tamanho_solucao; i++) {
		vertices_na_solucao[solucao[i]] = PRESENTE;
		custo -= (premios[solucao[i]]);
	}
	//somatório penaldades vértices não visitados
	for (i=0; i<n_vertices; i++) {
		if (!vertices_na_solucao[i]) {
			custo += penalidades[i];
		}
	}
	//somatório custo deslocamentos
	if (tamanho_solucao > 1) {
		for (i=0; i<tamanho_solucao -1; i++) {
			custo += matriz_distancias[i][i+1];
		}
	}
	
	free(vertices_na_solucao);
	return custo;
}

void imprime_vetor(int* vetor, int tamanho){
	int i;
	
	printf("\nVetor:\n");
	for (i=0; i<tamanho; i++) {
		printf("%d ", vetor[i]);
	}
	printf("\n");
}

int sorteia_candidato(int* custo_candidatos, float alpha, int n_vertices){
	int i;
	int lrc[n_vertices], tam_lrc = 0, min = INF, max = 0;
	
	for (i=0; i<n_vertices; i++) {
		if (custo_candidatos[i] == -1)
			continue;
		
		if (custo_candidatos[i] < min) {
			min = custo_candidatos[i];
		}
		if (custo_candidatos[i] > max) {
			max = custo_candidatos[i];
		}
	}
	
	printf("\n min: %d, max: %d\n", min, max);
	
	for (i=0; i<n_vertices; i++) {
		if (custo_candidatos[i] == -1)
			continue;
		
		if (custo_candidatos[i] <= min + alpha*(max - min)) {
			printf("\ncandidato %d entrou na LRC\n", i);
			lrc[tam_lrc] = i;
			tam_lrc++;
		}
	}
	
	int sorteado = lrc[rand()%tam_lrc];
	
	printf("\nSORTEADO: %d\n", sorteado);
	return sorteado;
}

float reativo(){ //FIXME grasp reativo aqui
	return ALPHA;
}

int main(int argc, const char * argv[]) {
	int i, j;
    int n_vertices = atoi(argv[1]);
    int premios[n_vertices];
	int penalidades[n_vertices];
    int **matriz_distancias = (int**) malloc(n_vertices * sizeof(int*));
	for (i=0; i<n_vertices; i++) {
		matriz_distancias[i] = (int*) malloc(n_vertices * sizeof(int));
	}
	
	inicia_funcaoaleatoria();
	
	le_premios(premios, n_vertices);
	le_penalidades(penalidades, n_vertices);
	le_matriz_distancias(matriz_distancias, n_vertices);
	
	int solucao[n_vertices], tamanho_solucao = 0, custo_solucao = INF, alpha, candidato_sorteado;
	int* vertices_na_solucao = NULL;
	vertices_na_solucao = zeros(vertices_na_solucao, n_vertices);
	
	adiciona_vertice_solucao(solucao, vertices_na_solucao, &tamanho_solucao, 0);
	
	while (tamanho_solucao < n_vertices) {
		
		int custo_candidatos[n_vertices];
		int solucao_temp[n_vertices], tamanho_solucao_temp, *vertices_na_solucao_temp = NULL;
		vertices_na_solucao_temp = zeros(vertices_na_solucao_temp, n_vertices);
		for (i=0; i<n_vertices; i++) {
			if (!vertices_na_solucao[i]) {
				tamanho_solucao_temp = tamanho_solucao;
				memcpy(solucao_temp, solucao, n_vertices*sizeof(int));
				adiciona_vertice_solucao(solucao_temp, vertices_na_solucao_temp, &tamanho_solucao_temp, i);
				custo_candidatos[i] = calcula_custo_solucao(solucao_temp, tamanho_solucao_temp, n_vertices, premios, penalidades, matriz_distancias);
			} else {
				custo_candidatos[i] = -1;
			}
		}
		
		for (i=0; i<n_vertices; i++) {
			printf("cand %d: %d\n", i, custo_candidatos[i]);
		}
		
		alpha = reativo();
		
		candidato_sorteado = sorteia_candidato(custo_candidatos, alpha, n_vertices);
		
		adiciona_vertice_solucao(solucao, vertices_na_solucao, &tamanho_solucao, candidato_sorteado);
		custo_solucao = calcula_custo_solucao(solucao, tamanho_solucao, n_vertices, premios, penalidades, matriz_distancias);
		
		free(vertices_na_solucao_temp);
		
	}
	
	printf("\nSOLUCAO:\n");
	imprime_vetor(solucao, n_vertices);
	
	printf("\nCUSTO: %d\n", custo_solucao);
	
	
	//liberando matriz de distancias da memória
	for (i=0; i<n_vertices; i++) {
		free(matriz_distancias[i]);
	}
	
	free(matriz_distancias);
	
	free(vertices_na_solucao);

    return 0;
}
