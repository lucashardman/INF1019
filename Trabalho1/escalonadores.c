#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "escalonadores.h"

#define maximo_programas 15 //Maximo de programas que podem ser executados simultaneamente

/* Public Functions */
//void escalonamentoPorPrioridade(int quantidadeProgramas, char *programas[maximo_programas], int prioridade);
//void escalonamentoRoundRobin(int quantidadeProgramas, char *programas[maximo_programas]);
//void escalonamentoLoteria(int quantidadeProgramas, char *programas[maximo_programas], int numeroTickets);

/* Static Functions */

/****************************************************************************
 * Politica de escalonamento por prioridade							*
 * Parametros:
 * quantidadeProgramas - quantidade de programas sendo gerenciados
 * *programas - lista de programas sendo gerenciados
 * prioridade -											*
 ****************************************************************************/
void escalonamentoPorPrioridade(int quantidadeProgramas, ProgramaPrioridade *programas[maximo_programas]){

	printf("Hello");
}

void escalonamentoRoundRobin(int quantidadeProgramas, ProgramaRoundRobin *programas[maximo_programas]){

	printf("Hey");
}

void escalonamentoLoteria(int quantidadeProgramas, ProgramaLoteria *programas[maximo_programas]){

	printf("Hi");
}

void helloWorld(){
	printf("Hello World!!");
}
