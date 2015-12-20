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
#include <math.h>
#include <sys/timeb.h>

#define TRUE 1
#define FALSE 0
#define ERRO -1
#define INF 9999999
#define PRESENTE 1
#define ALPHA 0.2
#define MIN_PREMIOS_COLETADOS 0.75

void inicia_funcaoaleatoria(void){
	struct timeb tmb;
	ftime(&tmb);
	srand((int)tmb.time*1000 + tmb.millitm);
}

int le_premios(int* premios, int n_vertices){
	int i;
	int somatorio_premios = 0;
	FILE *instancia;
	instancia = fopen("/Users/valberlaux/PCTSP/PCTSP/instancias/w10.txt", "r");
	
	if (instancia == NULL){
		printf("Erro ao carregar instância\n");
		exit(EXIT_FAILURE);
	}
	
	for (i=0; i<n_vertices; i++) {
		fscanf(instancia, "%d", &premios[i]);
		somatorio_premios += premios[i];
	}
	
	fclose(instancia);
	
	return somatorio_premios;
	
}

int le_penalidades(int* penalidades, int n_vertices){
	int i, somatorio_penalidades = 0;
	FILE* instancia = fopen("/Users/valberlaux/PCTSP/PCTSP/instancias/p10.txt", "r");
	
	if (instancia == NULL){
		printf("Erro ao carregar instância\n");
		exit(EXIT_FAILURE);
	}
	
	for (i=0; i<n_vertices; i++) {
		fscanf(instancia, "%d", &penalidades[i]);
		somatorio_penalidades += penalidades[i];
	}
	
	fclose(instancia);
	
	return somatorio_penalidades;
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

int* zeros(int tamanho){
	int i;
	int* vetor = malloc(tamanho*sizeof(int));
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

int calcula_custo_solucao(int* solucao, int tamanho_solucao, int somatorio_penalidades, int n_vertices, int* premios, int* penalidades, int** matriz_distancias){
	int custo;
	int i;
	
	custo = somatorio_penalidades;
	
	//descontando as penalidades dos vértices visitados
	for (i=0; i<tamanho_solucao; i++)
		custo -= penalidades[solucao[i]];
	
	//somatório custo deslocamentos
	for (i=0; i<tamanho_solucao -1; i++)
		custo += matriz_distancias[solucao[i]][solucao[i+1]];
	
	return custo;
}

void imprime_vetor(int* vetor, int tamanho, char* mensagem){
	int i;
	
	printf("\n%s\n", mensagem);
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
		
		if (custo_candidatos[i] <= (min + alpha*(max - min))) {
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

int encontra_posicao_vertice(int* vetor, int tamanho, int vertice){
	for (int i=0; i<tamanho; i++) {
		if (vetor[i] == vertice) {
			return i;
		}
	}
	return ERRO;
}

void remove_vertice(int* solucao, int* tamanho, int pos_vertice_removido){
	
	if(pos_vertice_removido != -1){
		for (int i= pos_vertice_removido; i<(*tamanho-1); i++) {
			solucao[i] = solucao[i+1];
		}
	}
		
	*tamanho = *tamanho-1;
}

int premios_coletados(int *solucao, int tamanho_solucao, int* premios){
	int premios_coletados = 0;
	for (int i = 0; i<tamanho_solucao; i++) {
		premios_coletados += premios[solucao[i]];
	}
	return premios_coletados;
}

int testa_viabilidade(int* solucao, int tamanho_solucao, int* premios, int min_premios_coletados){
	
	if (premios_coletados(solucao, tamanho_solucao, premios) >= min_premios_coletados)
		return TRUE;
	else
		return FALSE;
	
}

int calcula_custo_adicao_vertice_no_final(int* solucao, int tamanho_solucao, int id_vertice, int custo_solucao, int* premios, int* penalidades, int** matriz_distancias) {
	
	custo_solucao = custo_solucao - penalidades[id_vertice] + matriz_distancias[solucao[tamanho_solucao-1]][id_vertice];
	
	return custo_solucao;
}

int calcula_custo_remover_vertice(int* solucao, int pos, int custo_solucao, int tamanho_solucao, int* penalidades, int** matriz_distancias){

	custo_solucao = custo_solucao + penalidades[solucao[pos]] - matriz_distancias[solucao[pos-1]][solucao[pos]];
 
	if (pos != tamanho_solucao-1) {
		custo_solucao = custo_solucao - matriz_distancias[solucao[pos]][solucao[pos+1]] + matriz_distancias[solucao[pos-1]][solucao[pos+1]];
	}

	return custo_solucao;
}

int remove_vertice_menor_beneficio(int* solucao, int* custo_solucao, int* tamanho_solucao, int n_vertices, int* premios, int min_premios, int* penalidades, int** matriz_distancias){
	
	int i, temp, menor_custo = *custo_solucao, id_menor_custo = -1;
	int somatorio_premios_coletados = premios_coletados(solucao, *tamanho_solucao, premios);
	
	for (i=1; i<*tamanho_solucao; i++){
		temp = calcula_custo_remover_vertice(solucao, i, *custo_solucao, *tamanho_solucao, penalidades, matriz_distancias);
		if(temp < menor_custo && (somatorio_premios_coletados - premios[solucao[i]]) >= min_premios){
			menor_custo = temp;
			id_menor_custo = i;
		}
	}
	
	if (id_menor_custo != ERRO){
		remove_vertice(solucao, tamanho_solucao, id_menor_custo);
		*custo_solucao = menor_custo;
		printf("Removi o vértice %d na pos %d com custo %d\n", solucao[id_menor_custo], id_menor_custo, menor_custo);
		return TRUE;
	}
	else
		return FALSE;
}

int main(int argc, const char * argv[]) {
	
	inicia_funcaoaleatoria();

	//variáveis do problema
    int n_vertices = atoi(argv[1]);
	int *premios = malloc(n_vertices*sizeof(int));
	int *penalidades = malloc(n_vertices*sizeof(int));
	int *solucao = malloc(n_vertices*sizeof(int)), tamanho_solucao = 0, custo_solucao = INF, candidato_sorteado;
	float alpha;
	int *vertices_na_solucao = zeros(n_vertices);
	
	int somatorio_premios, somatorio_penalidades, min_premios;
	
	//variáveis temporárias
	int i;
	int custo_candidatos[n_vertices];
	
	//alocação vetor de distâncias
    int **matriz_distancias = (int**) malloc(n_vertices * sizeof(int*));
	for (i=0; i<n_vertices; i++) {
		matriz_distancias[i] = (int*) malloc(n_vertices * sizeof(int));
	}
	
	//leitura da instância
	somatorio_premios = le_premios(premios, n_vertices);
	min_premios = (int) ceil(0.75 * somatorio_premios);
	printf("MINIMO PREMIOS EXIGIDOS É: %d\n", min_premios);
	somatorio_penalidades = le_penalidades(penalidades, n_vertices);
	le_matriz_distancias(matriz_distancias, n_vertices);
	
	
	//construção GRASP
	
	adiciona_vertice_solucao(solucao, vertices_na_solucao, &tamanho_solucao, 0);
	custo_solucao = calcula_custo_solucao(solucao, tamanho_solucao, somatorio_penalidades, n_vertices, premios, penalidades, matriz_distancias);
	
	while (tamanho_solucao < n_vertices) {
		
		for (i=0; i<n_vertices; i++) {
			if (!vertices_na_solucao[i]) {
				custo_candidatos[i] = calcula_custo_adicao_vertice_no_final(solucao, tamanho_solucao, i, custo_solucao, premios, penalidades, matriz_distancias);
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
		
		imprime_vetor(solucao, tamanho_solucao, "solucao construida até aqui:");
		
		custo_solucao = custo_candidatos[candidato_sorteado];
		
	}
	
	if (testa_viabilidade(solucao, tamanho_solucao, premios, min_premios)) {
		printf("Solucao viável!\n");
	} else {
		printf("Solucao inviável :( \n");
	}
	//fim-construção
	
	
	
	//VNS
	
	/*
	int k = 1, k_max = 4;
	memcpy(solucao_temp, solucao, n_vertices*sizeof(int));
	
	while (k<=k_max) {
		switch (k) {
			case 1:
				//remove_vertices();
				break;
			
			
			default:
				printf("Erro na estrutura de vizinhança do VNS\n");
				exit(EXIT_FAILURE);
				break;
		}
		
		//busca_local(solucao_temp);
		
		custo_solucao_temp = calcula_custo_solucao(solucao_temp, tamanho_solucao_temp, n_vertices, premios, min_premios, penalidades, matriz_distancias);
		if (custo_solucao_temp < custo_solucao) {
			free(solucao);
			solucao = solucao_temp;
			custo_solucao = custo_solucao_temp;
			k = 1;
		} else {
			k++;
		}
		
	}
	*/

	imprime_vetor(solucao, tamanho_solucao, "solucao:");
	printf("\ncusto final: %d\n", custo_solucao);
	
	printf("\n Confirmando cálculo do custo: %d\n", calcula_custo_solucao(solucao, tamanho_solucao, somatorio_penalidades, n_vertices, premios, penalidades, matriz_distancias));
	
	while (remove_vertice_menor_beneficio(solucao, &custo_solucao, &tamanho_solucao, n_vertices, premios, min_premios, penalidades, matriz_distancias)){
		imprime_vetor(solucao, tamanho_solucao, "vetor:");
		printf("custo apos remocao: %d\n", custo_solucao);
	};
	
	imprime_vetor(solucao, tamanho_solucao, "melhor solucao viável");
	
	printf("\ntamanho_solucao: %d\ncusto depois de remover: %d\n", tamanho_solucao, custo_solucao);
	
	printf("confirmando custo: %d\n", calcula_custo_solucao(solucao, tamanho_solucao, somatorio_penalidades, n_vertices, premios, penalidades, matriz_distancias));
	
	
	//liberando matriz de distancias da memória
	for (i=0; i<n_vertices; i++) {
		free(matriz_distancias[i]);
	}
	free(matriz_distancias);
	free(vertices_na_solucao);
	//free(solucao); @FIXME problemas com redefinicao de ponteiros no meio do código
	free(premios);
	free(penalidades);

    return 0;
}
