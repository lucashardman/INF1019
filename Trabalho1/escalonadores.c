#include <signal.h>
#include <unistd.h>

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

	int loop1 = 0, loop2; //Variaveis auxiliares para loop
	int pid[maximo_programas], pid_auxiliar = 0; //Variaveis responsaveis por guardar os pid dos processos

	/* Inicializar o campo "terminado" com false */
	for(loop1=0;loop1<quantidadeProgramas;loop1++){
		programas[loop1]->terminado = false;
	}
	/*TESTAR ESTA PARTE DO CODIGO*/
	for(loop1=0;loop1<quantidadeProgramas;loop1++){
		
		//printf("%s\n", programas[loop1]->nome);

		pid_auxiliar = fork();

		if(pid_auxiliar!=0){
			pid[loop1] = pid_auxiliar;
			kill(pid_auxiliar, SIGSTOP);
		}
		else{
			sleep(1);
			execve(programas[loop1]->nome,NULL,NULL);
		}
	}

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