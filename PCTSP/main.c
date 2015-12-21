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
#define N_VIZINHANCAS 5

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

void adiciona_vertice_final_solucao(int *solucao, int* vertices_na_solucao, int *tamanho_solucao, int vertice){
	solucao[*tamanho_solucao] = vertice;
	vertices_na_solucao[vertice] = 1;
	*tamanho_solucao = *tamanho_solucao + 1;
}

int calcula_custo_solucao(int* solucao, int tamanho_solucao, int somatorio_penalidades, int n_vertices, int* penalidades, int** matriz_distancias){ //corrigido para volta
	int custo;
	int i;
	
	custo = somatorio_penalidades;
	
	//descontando as penalidades dos vértices visitados
	for (i=0; i<tamanho_solucao; i++)
		custo -= penalidades[solucao[i]];
	
	//somatório custo deslocamentos
	for (i=0; i<tamanho_solucao -1; i++)
		custo += matriz_distancias[solucao[i]][solucao[i+1]];
		
	custo += matriz_distancias[solucao[0]][solucao[tamanho_solucao-1]]; // custo da volta para o vértice inicial
	
	return custo;
}

int gera_numero_aleatorio_entre(int limite_inferior, int limite_superior){
	if (limite_inferior == limite_superior) {
		return limite_superior;
	}
	int	numero_aleatorio = rand()%(limite_superior-limite_inferior);
	numero_aleatorio += limite_inferior;
	
	if (numero_aleatorio == 0) {
		printf("QUE PORRA É ESSA???!!!!! não tem como ser 0 mdds!\n");
		exit(EXIT_FAILURE);
	}

	return numero_aleatorio;
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

void remove_vertice(int* solucao, int* tamanho, int pos_vertice_removido, int* vertices_na_solucao){
	
	vertices_na_solucao[solucao[pos_vertice_removido]] = FALSE;
	
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

int calcula_custo_adicao_vertice_no_final(int* solucao, int tamanho_solucao, int id_vertice, int custo_solucao, int* penalidades, int** matriz_distancias) { //corrigido para a volta
	
	custo_solucao = custo_solucao - penalidades[id_vertice] - matriz_distancias[solucao[tamanho_solucao-1]][0] + matriz_distancias[solucao[tamanho_solucao-1]][id_vertice] + matriz_distancias[id_vertice][0];
	
	return custo_solucao;
}

int calcula_custo_adicionar_vertice(int* solucao, int vertice, int pos, int custo_solucao, int tamanho_solucao, int* penalidades, int** matriz_distancias){ //corrigido para volta
	
	if (pos == tamanho_solucao-1) {
		return calcula_custo_adicao_vertice_no_final(solucao, tamanho_solucao, vertice, custo_solucao, penalidades, matriz_distancias);
	} else {
		custo_solucao = custo_solucao - penalidades[vertice] + matriz_distancias[solucao[pos-1]][vertice] - matriz_distancias[solucao[pos-1]][solucao[pos]] + matriz_distancias[vertice][solucao[pos]];
	}
	
	return custo_solucao;
}

int calcula_custo_remover_vertice(int* solucao, int pos, int custo_solucao, int tamanho_solucao, int* penalidades, int** matriz_distancias){ //corrigido para volta

	custo_solucao = custo_solucao + penalidades[solucao[pos]] - matriz_distancias[solucao[pos-1]][solucao[pos]];
 
	if (pos != tamanho_solucao-1) {
		custo_solucao = custo_solucao - matriz_distancias[solucao[pos]][solucao[pos+1]] + matriz_distancias[solucao[pos-1]][solucao[pos+1]];
	}
	if (pos == tamanho_solucao-1) {
		custo_solucao = custo_solucao - matriz_distancias[solucao[pos]][0] + matriz_distancias[solucao[pos-1]][0];
	}

	return custo_solucao;
}

int remove_vertice_menor_beneficio(int* solucao, int* custo_solucao, int* tamanho_solucao, int* vertices_na_solucao, int n_vertices, int* premios, int min_premios, int* penalidades, int** matriz_distancias){
	
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
		remove_vertice(solucao, tamanho_solucao, id_menor_custo, vertices_na_solucao);
		*custo_solucao = menor_custo;
		printf("Removi o vértice %d na pos %d com custo %d\n", solucao[id_menor_custo], id_menor_custo, menor_custo);
		if(!testa_viabilidade(solucao, *tamanho_solucao, premios, min_premios)){
			printf("FIZ MERDA NÃO É VIÁVEL!!!!!!!\n");
			exit(EXIT_FAILURE);
		}
		return TRUE;
	}
	else
		return FALSE;
}

void adiciona_vertice(int* solucao, int *tamanho_solucao, int posicao, int vertice, int* vertices_na_solucao){
	for (int i = *tamanho_solucao; i > posicao; i--) {
		solucao[i] = solucao[i-1];
	}
	solucao[posicao] = vertice;
	vertices_na_solucao[vertice] = TRUE;
	*tamanho_solucao = *tamanho_solucao + 1;
}

int adiciona_vertice_melhor_beneficio(int* solucao, int *tamanho_solucao, int *custo_solucao, int n_vertices, int* vertices_na_solucao, int* penalidades, int** matriz_distancias){
	int melhor_vertice = -1, melhor_posicao = -1, melhor_custo = INF, temp = INF;
	
	if (*tamanho_solucao == n_vertices) // já estão todos na solução, ninguém para inserir.
		return FALSE;
	
	for (int i = 0; i < n_vertices; i++) { // varre os vértices para ver quais estão livres
		if (!vertices_na_solucao[i]) {
			for (int j = 1; j<*tamanho_solucao; j++) { // a cada vértice livre encontrado, testa o custo de inseri-lo em todas as posicoes da solucao
				temp = calcula_custo_adicionar_vertice(solucao, i, j, *custo_solucao, *tamanho_solucao, penalidades, matriz_distancias);
				if (temp < melhor_custo) {
					melhor_custo = temp;
					melhor_vertice = i;
					melhor_posicao = j;
				}
			}
		}
	}
	
	if(melhor_custo < *tamanho_solucao){
		adiciona_vertice(solucao, tamanho_solucao, melhor_posicao, melhor_vertice, vertices_na_solucao);
		*custo_solucao = melhor_custo;
		return TRUE;
	} else {
		return FALSE;
	}
}

int adiciona_vertice_aleatorio(int* solucao, int* tamanho_solucao, int* custo_solucao, int n_vertices, int* vertices_na_solucao, int* penalidades, int** matriz_distancias){
	
	int id_vertice_adicionar = -1, posicao = -1;
	
	if (*tamanho_solucao == n_vertices) {
		return ERRO;
	}
	
	do {
		id_vertice_adicionar = rand()%n_vertices;
	} while (vertices_na_solucao[id_vertice_adicionar]);
	
	do {
		posicao = rand()%*tamanho_solucao;
	} while (posicao == 0);
	
	*custo_solucao = calcula_custo_adicionar_vertice(solucao, id_vertice_adicionar, posicao, *custo_solucao, *tamanho_solucao, penalidades, matriz_distancias);
	adiciona_vertice(solucao, tamanho_solucao, posicao, id_vertice_adicionar, vertices_na_solucao);
	return TRUE;
}

void vnd_busca5(int* solucao, int *tamanho_solucao, int n_vertices, int *custo_solucao, int* vertices_na_solucao, int* penalidades, int* premios, int min_premios, int** matriz_distancias){
	int melhor_pos = -1, melhor_custo = INF, temp, pos_vertice_remover;
	
	int premios_coletados_solucao = premios_coletados(solucao, *tamanho_solucao, premios);
	
	for (int i = 0; i<n_vertices; i++) {
		pos_vertice_remover = gera_numero_aleatorio_entre(1, *tamanho_solucao);
		temp = calcula_custo_remover_vertice(solucao, pos_vertice_remover, *custo_solucao, *tamanho_solucao, penalidades, matriz_distancias);
		if (temp < melhor_pos && (premios_coletados_solucao - premios[solucao[pos_vertice_remover]] >= min_premios)){
			melhor_custo = temp;
			melhor_pos = pos_vertice_remover;
		}
	}
	if (melhor_custo < *custo_solucao) {
		*custo_solucao = melhor_custo;
		remove_vertice(solucao, tamanho_solucao, melhor_pos, vertices_na_solucao);
	}
}

int calcula_custo_troca_posicoes(int* solucao, int tamanho_solucao, int custo_solucao, int** matriz_distancias, int posicao1, int posicao2){ //corrigida para a volta
	if (posicao1 == posicao2) {
		return custo_solucao;
	}
	else if (posicao2 < posicao1){ // ordena para posicao2 ser maior que posicao1 (útil nas contas abaixo)
		int temp = posicao1;
		posicao1 = posicao2;
		posicao2 = temp;
	}
	
	custo_solucao = custo_solucao - matriz_distancias[solucao[posicao1-1]][solucao[posicao1]] + matriz_distancias[solucao[posicao1-1]][solucao[posicao2]];
	
	if (posicao2 != tamanho_solucao - 1) { //se for a final não executa essas contas
		custo_solucao = custo_solucao - matriz_distancias[solucao[posicao2]][solucao[posicao2+1]] + matriz_distancias[solucao[posicao1]][solucao[posicao2+1]];
	}
	if (posicao2 == tamanho_solucao -1) { // se for última, recalcula o preco da volta para a origem
		custo_solucao = custo_solucao - matriz_distancias[solucao[posicao2]][0] + matriz_distancias[solucao[posicao1]][0];

	}
	
	if ((posicao2-posicao1) != 1) { // se forem vizinhos não executa essas
		custo_solucao = custo_solucao - matriz_distancias[solucao[posicao2-1]][solucao[posicao2]] - matriz_distancias[solucao[posicao1]][solucao[posicao1 + 1]];
		custo_solucao = custo_solucao + matriz_distancias[solucao[posicao2-1]][solucao[posicao1]] + matriz_distancias[solucao[posicao2]][solucao[posicao1+1]];
	}
	
	return custo_solucao;
}

void troca_duas_posicoes(int* solucao, int tamanho_solucao, int *custo_solucao, int n_vertices, int* penalidades, int somatorio_penalidades, int** matriz_distancias, int posicao1, int posicao2){
	if (posicao1 == posicao2) {
		printf("Posições iguais: não troquei nada\n");
		return;
	}
	
	printf("troquei a posicao %d com a posicao %d\n", posicao1, posicao2);
	
	*custo_solucao = calcula_custo_troca_posicoes(solucao, tamanho_solucao, *custo_solucao, matriz_distancias, posicao1, posicao2);
	
	int temp = solucao[posicao1];
	solucao[posicao1] = solucao[posicao2];
	solucao[posicao2] = temp;
	
	printf("E o custo ficou em %d\n", *custo_solucao);
	
}

void contrucao_grasp(int* solucao, int n_vertices, int* custo_solucao, int* tamanho_solucao, int* vertices_na_solucao, int** matriz_distancias, int* premios, int min_premios, int* penalidades, int somatorio_penalidades){
	
	//inicia solução sempre com o vértice obrigatório 0
	adiciona_vertice_final_solucao(solucao, vertices_na_solucao, tamanho_solucao, 0);
	*custo_solucao = calcula_custo_solucao(solucao, *tamanho_solucao, somatorio_penalidades, n_vertices, penalidades, matriz_distancias);
	int custo_candidatos[n_vertices];
	
	while (*tamanho_solucao < n_vertices) {
		
		for (int i=0; i<n_vertices; i++) {
			if (!vertices_na_solucao[i]) {
				custo_candidatos[i] = calcula_custo_adicao_vertice_no_final(solucao, *tamanho_solucao, i, *custo_solucao, penalidades, matriz_distancias);
			} else {
				custo_candidatos[i] = -1;
			}
		}
		
		for (int i=0; i<n_vertices; i++) {
			printf("cand %d: %d\n", i, custo_candidatos[i]);
		}
		
		float alpha = reativo();
		
		int candidato_sorteado = sorteia_candidato(custo_candidatos, alpha, n_vertices);
		
		adiciona_vertice_final_solucao(solucao, vertices_na_solucao, tamanho_solucao, candidato_sorteado);
		
		imprime_vetor(solucao, *tamanho_solucao, "solucao construida até aqui:");
		
		*custo_solucao = custo_candidatos[candidato_sorteado];
		
	}
	
	if (testa_viabilidade(solucao, *tamanho_solucao, premios, min_premios)) {
		printf("Solucao viável!\n");
	} else {
		printf("Solucao inviável :( \n");
	}

	
}

void vnd_busca3(int* solucao, int* custo_solucao, int* tamanho_solucao, int n_vertices, int* penalidades, int somatorio_penalidades, int** matriz_distancias){
	int posicao1;
	int posicao2;
	
	int melhor_pos1 = -1, melhor_pos2 = -1, melhor_custo = *custo_solucao, temp = INF;
	
	for (int i = 0; i<1000; i++) {
		posicao1 = gera_numero_aleatorio_entre(1, *tamanho_solucao);
		posicao2 = gera_numero_aleatorio_entre(1, *tamanho_solucao);
		
		temp = calcula_custo_troca_posicoes(solucao, *tamanho_solucao, *custo_solucao, matriz_distancias, posicao1, posicao2);
		
		if(temp < melhor_custo) {
			melhor_custo = temp;
			melhor_pos1 = posicao1;
			melhor_pos2 = posicao2;
		}
	}
	
	if (melhor_custo < *custo_solucao) {
		troca_duas_posicoes(solucao, *tamanho_solucao, custo_solucao, n_vertices, penalidades, somatorio_penalidades, matriz_distancias, melhor_pos1, melhor_pos2);
	}
	
}

void vnd_busca4(int* solucao, int* tamanho_solucao, int* custo_solucao, int n_vertices, int* vertices_na_solucao, int* penalidades, int* premios, int** matriz_distancias){ // adiciona aleatório
	int n_vertices_fora = n_vertices - *tamanho_solucao;
	
	int vertices_fora[n_vertices_fora], prox_pos = 0, posicao_adicao, vertice_adicionar, melhor_pos = -1, melhor_vertice = -1, melhor_custo = INF, temp;
	
	for (int i = 0; i<n_vertices; i++) {
		if (!vertices_na_solucao[i]) {
			vertices_fora[prox_pos] = i;
			prox_pos++;
		}
	}
	
	for (int i = 0; i<n_vertices; i++) {
		posicao_adicao = gera_numero_aleatorio_entre(0, *tamanho_solucao);
		vertice_adicionar = gera_numero_aleatorio_entre(0, n_vertices_fora);
		temp = calcula_custo_adicionar_vertice(solucao, vertices_fora[vertice_adicionar], posicao_adicao, *custo_solucao, *tamanho_solucao, penalidades, matriz_distancias);
		if (temp < melhor_custo){
			melhor_custo = temp;
			melhor_pos = posicao_adicao;
			melhor_vertice = vertice_adicionar;
		}
	}
	
	if (melhor_custo < *custo_solucao) {
		adiciona_vertice(solucao, tamanho_solucao, melhor_pos, melhor_vertice, vertices_na_solucao);
		*custo_solucao = melhor_custo;
	}
}

void vnd_busca1(int* solucao, int* custo_solucao, int* tamanho_solucao, int* vertices_na_solucao, int n_vertices, int* premios, int min_premios, int* penalidades, int** matriz_distancias){ // remove enquanto melhorar
	while (remove_vertice_menor_beneficio(solucao, custo_solucao, tamanho_solucao, vertices_na_solucao, n_vertices, premios, min_premios, penalidades, matriz_distancias));
}

void vnd_busca2(int* solucao, int* tamanho_solucao, int* custo_solucao, int n_vertices, int* vertices_na_solcuao, int* penalidades, int** matriz_distancias){ // adiciona enquanto melhorar
	while (adiciona_vertice_melhor_beneficio(solucao, tamanho_solucao, custo_solucao, n_vertices, vertices_na_solcuao, penalidades, matriz_distancias));
}


int main(int argc, const char * argv[]) {
	
	inicia_funcaoaleatoria();

	//variáveis do problema
    int n_vertices = atoi(argv[1]);
	int *premios = malloc(n_vertices*sizeof(int));
	int *penalidades = malloc(n_vertices*sizeof(int));
	int *solucao = malloc(n_vertices*sizeof(int)), tamanho_solucao = 0, custo_solucao = INF;
	int *vertices_na_solucao = zeros(n_vertices);
	
	int somatorio_premios, somatorio_penalidades, min_premios;
	
	//variáveis temporárias
	int i;
	
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
	contrucao_grasp(solucao, n_vertices, &custo_solucao, &tamanho_solucao, vertices_na_solucao, matriz_distancias, premios, min_premios, penalidades, somatorio_penalidades);
	
	printf("custo apos construcao: %d\n", custo_solucao);
	
	//nd_busca1(solucao, &custo_solucao, &tamanho_solucao, vertices_na_solucao, n_vertices, premios, min_premios, penalidades, matriz_distancias);
	
	//vnd_busca2(solucao, &tamanho_solucao, &custo_solucao, n_vertices, vertices_na_solucao, penalidades, matriz_distancias);
	
	vnd_busca3(solucao, &custo_solucao, &tamanho_solucao, n_vertices, penalidades, somatorio_penalidades, matriz_distancias);
	
	imprime_vetor(solucao, tamanho_solucao, "apos busca local");

	printf("custo solucao: %d\nconfirmando custo: %d\n", custo_solucao, calcula_custo_solucao(solucao, tamanho_solucao, somatorio_penalidades, n_vertices, penalidades, matriz_distancias));
	
	//VND
	/*
	int l =  0, l_max = N_VIZINHANCAS;
	while (l < l_max) {
		switch (l) {
			case 0:
				break;
				
			default:
    break;
		}
	}
	*/
	
	
	
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
	
	printf("\n Confirmando cálculo do custo: %d\n", calcula_custo_solucao(solucao, tamanho_solucao, somatorio_penalidades, n_vertices, penalidades, matriz_distancias));
	
	/*vnd_busca1(solucao, &custo_solucao, &tamanho_solucao, vertices_na_solucao, n_vertices, premios, min_premios, penalidades, matriz_distancias);
	imprime_vetor(solucao, tamanho_solucao, "APÓS BL1:\n");
	
	printf("CUSTO SOLUCAO APOS BL1: %d\n Confirmando custo: %d\n", custo_solucao, calcula_custo_solucao(solucao, tamanho_solucao, somatorio_penalidades, n_vertices, penalidades, matriz_distancias));
	*/
	 
	/*
	while (remove_vertice_menor_beneficio(solucao, &custo_solucao, &tamanho_solucao, vertices_na_solucao, n_vertices, premios, min_premios, penalidades, matriz_distancias)){
		imprime_vetor(solucao, tamanho_solucao, "vetor:");
		printf("custo apos remocao: %d\n", custo_solucao);
	};
	
	imprime_vetor(solucao, tamanho_solucao, "melhor solucao viável");
	
	printf("\ntamanho_solucao: %d\ncusto depois de remover: %d\n", tamanho_solucao, custo_solucao);
	
	printf("confirmando custo: %d\n", calcula_custo_solucao(solucao, tamanho_solucao, somatorio_penalidades, n_vertices, penalidades, matriz_distancias));
	
	adiciona_vertice_melhor_beneficio(solucao, &tamanho_solucao, &custo_solucao, n_vertices, vertices_na_solucao, penalidades, matriz_distancias);
	
	imprime_vetor(solucao, tamanho_solucao, "Solucao após inserir um vértice de volta");
	
	printf("tamanho: %d\ncusto solucao: %d\n confirmando custo solucao: %d\n", tamanho_solucao, custo_solucao, calcula_custo_solucao(solucao, tamanho_solucao, somatorio_penalidades, n_vertices, penalidades, matriz_distancias));
	*/
	
	
	/*
	troca_duas_posicoes(solucao, tamanho_solucao, &temp, n_vertices, penalidades, somatorio_penalidades, matriz_distancias);
	imprime_vetor(solucao, tamanho_solucao, "Depois de trocar as posicoes");
	printf("Confirmando o calculo do custo: %d\n", calcula_custo_solucao(solucao, tamanho_solucao, somatorio_penalidades, n_vertices, penalidades, matriz_distancias));
	*/
	//liberando matriz de distancias da memória
	for (i=0; i<n_vertices; i++) {
		free(matriz_distancias[i]);
	}
	free(matriz_distancias);
	free(solucao);
	free(premios);
	free(penalidades);
	free(vertices_na_solucao);

    return 0;
}
