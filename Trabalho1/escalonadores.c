#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

#include "escalonadores.h"

/* Variaveis globais */
static int metodoEscalonamento;
/* Funcoes estaticas */
static void iniciarProgramas(int quantidadeProgramas, char nome[15][maximo_programas], int *pid);
static bool testaProgramasFinalizados(int quantidadeProgramas, void *programas[maximo_programas]);

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

	char stringAux[15][maximo_programas]; //Variavel auxiliar para copias de strings

	metodoEscalonamento = 2;

	/* Inicializar o campo "terminado" com false */
	for(loop1=0;loop1<quantidadeProgramas;loop1++){
		programas[loop1]->terminado = false;
	}

	/* Executa os programas e envia o sinal para entrarem em espera */
	for(loop1=0;loop1<quantidadeProgramas;loop1++){
		strcpy(stringAux[loop1], programas[loop1]->nome);
	}
	iniciarProgramas(quantidadeProgramas, stringAux, pid);
	/* Fim: Executa os programas e envia o sinal para entrarem em espera */

	for(loop1=0;loop1<quantidadeProgramas;loop1++){
		printf("O programa: %s de pid %d foi iniciado.\n", programas[loop1]->nome, pid[loop1]);
	}

	/* Inicio do algoritmo de Round-Robin */
	while(1){
		if(testaProgramasFinalizados(quantidadeProgramas, (void*)programas) == true)
			return; //Finaliza a funcao de Round-Robin
	}
	/* Fim do algoritmo de Round-Robin*/
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

/****************************** Funcoes estaticas ***************************/

/****************************************************************************
 * Nome: iniciarProgramas               									*
 * Descricao: responsavel por iniciar os programas que serao escalonados    *
 * utilizando execve e enviar um sinal para eles entrarem em espera SIGSTOP *
 * Parametros:																*
 * quantidadeProgramas - quantidade de programas sendo gerenciados			*
 * nome - vetor com o nome dos programas que estao sendo gerenciados		*
 * pid - vetor responsavel por retornar o pid dos programas                 *
 ****************************************************************************/
static void iniciarProgramas(int quantidadeProgramas, char nome[15][maximo_programas], int *pid){

	int loop = 0;
	int pid_auxiliar = 0; //Variaveis responsaveis por guardar os pid dos processos

	/* Executa os programas e envia o sinal para entrarem em espera */
	for(loop=0;loop<quantidadeProgramas;loop++){

		pid_auxiliar = fork();

		if(pid_auxiliar!=0){

			pid[loop] = pid_auxiliar;
			kill(pid_auxiliar, SIGSTOP);
			printf("Estou aqui\n");
		}
		else{
			printf("Programa %s iniciado.\n",nome[loop]); //Para este print aparecer, comentar o kill
			sleep(3); //Programas iniciados com uma diferenca de 3 segundos entre cada
			execve(nome[loop],NULL,NULL);
			
		}
	}
	/* FIM: Executa os programas e envia o sinal para entrarem em espera */
}

/****************************************************************************
 * Nome: testaProgramasFinalizados         									*
 * Descricao: responsavel por testar se todos os programas ja foram         *
 * finalizados.                                                             *
 * Parametros:																*
 * quantidadeProgramas - quantidade de programas sendo gerenciados			*
 * programas - vetor com os programas que estao sendo gerenciados	    	*
 * Condicoes de retorno:                                                    *
 * true - todos os programas ja foram finalizados                           *
 * false - ainda faltam programas a serem finalizados                       *
 ****************************************************************************/
static bool testaProgramasFinalizados(int quantidadeProgramas, void *programas[maximo_programas]){

	int loop = 0;
	int contadorTerminados = 0;

	/************************
	 *Bloco de teste:       */
	 printf("Teste de alcance para funcao testaProgramasFinalizados");

	 printf("Lista de programas:\n");
	 for(loop=0;loop<quantidadeProgramas;loop++)
	 	printf("%s\n", ((ProgramaRoundRobin)programas[loop])->nome);
	/************************/

/*
	for(loop=0;loop<quantidadeProgramas;loop++){
		
		if(metodoEscalonamento == 1){
			if((ProgramaPrioridade)programas[loop]->terminado == false){
				contadorTerminados++;
			}
		}
		if(metodoEscalonamento == 2){
			if((ProgramaRoundRobin)programas[loop]->terminado == false){
				contadorTerminados++;
			}
		}
		if(metodoEscalonamento == 3){
			if((ProgramaLoteria)programas[loop]->terminado == false){
				contadorTerminados++;
			}
		}
	}
*/
	if(contadorTerminados == quantidadeProgramas){
		return false;
	}

	return true;
}