#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "escalonadores.h"

/* Funcoes staticas */

/* Funcoes de escalonamento */

/****************************************************************************
 * Politica de escalonamento por prioridade									*
 * Parametros:																*
 * quantidadeProgramas - quantidade de programas sendo gerenciados			*
 * *programas - lista de programas sendo gerenciados						*
 ****************************************************************************/
void escalonamentoPorPrioridade(int quantidadeProgramas, ProgramaPrioridade *programas[maximo_programas]){

	printf("Hello\n");
}

/****************************************************************************
 * Politica de escalonamento Round-Robin									*
 * Parametros:																*
 * quantidadeProgramas - quantidade de programas sendo gerenciados			*
 * *programas - lista de programas sendo gerenciados						*
 ****************************************************************************/
void escalonamentoRoundRobin(int quantidadeProgramas, ProgramaRoundRobin *programas[maximo_programas]){

	printf("Hey\n");
}

/****************************************************************************
 * Politica de escalonamento por Loteria									*
 * Parametros:																*
 * quantidadeProgramas - quantidade de programas sendo gerenciados			*
 * *programas - lista de programas sendo gerenciados						*
 ****************************************************************************/
void escalonamentoLoteria(int quantidadeProgramas, ProgramaLoteria *programas[maximo_programas]){

	printf("Hi\n");
}