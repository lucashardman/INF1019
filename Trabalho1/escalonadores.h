#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define maximo_programas 15 //Maximo de programas que podem ser executados simultaneamente

/* Estruturas com os dados lidos no arquivo exec.txt */

typedef struct programaPrioridade{
	char execStr[5]; //String "Exec"
	char nome[15]; //Nome do programa
	int prioridade; //Prioridade do programa
	float tempoExecucao; //Tempo que demorou a execucao do programa em milissegundo
	bool terminado; //True se o programa terminou de executar ou false se ele ainda esta executando
}ProgramaPrioridade;

typedef struct programaRoundRobin{
	char execStr[5]; //String "Exec"
	char nome[15]; //Nome do programa
	float tempoExecucao; //Tempo que demorou a execucao do programa em milissegundo
	bool terminado; //True se o programa terminou de executar ou false se ele ainda esta executando
}ProgramaRoundRobin;

typedef struct programaLoteria{
	char execStr[5]; //String "Exec"
	char nome[15]; //Nome do programa
	int quantidadeBilhetes; //Quantidade de bilhetes que cada programa tem
	int bilhetes[20]; //Os bilhetes de cada programa
	float tempoExecucao; //Tempo que demorou a execucao do programa em milissegundo
	bool terminado; //True se o programa terminou de executar ou false se ele ainda esta executando
}ProgramaLoteria;

/* Funcoes responsaveis pelos algoritmos de escalonamento */

void escalonamentoPorPrioridade(int quantidadeProgramas, ProgramaPrioridade *programas[maximo_programas]);

void escalonamentoRoundRobin(int quantidadeProgramas, ProgramaRoundRobin *programas[maximo_programas]);

void escalonamentoLoteria(int quantidadeProgramas, ProgramaLoteria *programas[maximo_programas]);