#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

#include "escalonadores.h"

/* Variaveis globais */
static int metodoEscalonamento;

static ProgramaPrioridade *progPrioridade[maximo_programas]; //Variavel responsavel pelo controle dos programas pela politica por prioridade
static ProgramaRoundRobin *progRoundRobin[maximo_programas]; //Variavel responsavel pelo controle dos programas pela politica por round robin
static ProgramaLoteria *progLoteria[maximo_programas]; //Variavel responsavel pelo controle dos programas pela politica por loteria

/* Funcoes estaticas */
static void iniciarProgramas(int quantidadeProgramas, int *pid);
static bool testaProgramasFinalizados(int quantidadeProgramas);

/* Funcoes de escalonamento */

/****************************************************************************
 * Politica de escalonamento por prioridade                                 *
 * Parametros:                                                              *
 * quantidadeProgramas - quantidade de programas sendo gerenciados          *
 * *programas - lista de programas sendo gerenciados                        *
 ****************************************************************************/
void escalonamentoPorPrioridade(int quantidadeProgramas, ProgramaPrioridade *programas[maximo_programas]){

	printf("Hello\n");
}

/****************************************************************************
 * Politica de escalonamento Round-Robin                                    *
 * Parametros:                                                              *
 * quantidadeProgramas - quantidade de programas sendo gerenciados          *
 * *programas - lista de programas sendo gerenciados                        *
 ****************************************************************************/
void escalonamentoRoundRobin(int quantidadeProgramas, ProgramaRoundRobin *programas[maximo_programas]){

	int loop1 = 0, loop2; //Variaveis auxiliares para loop
	int pid[maximo_programas], pid_auxiliar = 0; //Variaveis responsaveis por guardar os pid dos processos

	metodoEscalonamento = 2;

	/* Executa os programas e envia o sinal para entrarem em espera */
	for(loop1=0;loop1<quantidadeProgramas;loop1++){
		progRoundRobin[loop1] = programas[loop1];
	}
	iniciarProgramas(quantidadeProgramas, pid);
	/* Fim: Executa os programas e envia o sinal para entrarem em espera */

	/* Inicializar o campo "terminado" com false */
	for(loop1=0;loop1<quantidadeProgramas;loop1++){
		progRoundRobin[loop1]->terminado = false;
	}

	for(loop1=0;loop1<quantidadeProgramas;loop1++){
		printf("O programa: %s de pid %d foi iniciado.\n", progRoundRobin[loop1]->nome, pid[loop1]);
	}

	/* Inicio do algoritmo de Round-Robin */
	while(1){
		if(testaProgramasFinalizados(quantidadeProgramas) == true) // Testa se todos os programas ja foram finalizados
			return; //Finaliza a funcao de Round-Robin


	}
	/* Fim do algoritmo de Round-Robin*/
}

/****************************************************************************
 * Politica de escalonamento por Loteria                                    *
 * Parametros:                                                              *
 * quantidadeProgramas - quantidade de programas sendo gerenciados          *
 * *programas - lista de programas sendo gerenciados                        *
 ****************************************************************************/
void escalonamentoLoteria(int quantidadeProgramas, ProgramaLoteria *programas[maximo_programas]){

	printf("Hi\n");
}

/****************************** Funcoes estaticas ***************************/

/****************************************************************************
 * Nome: iniciarProgramas                                                   *
 * Descricao: responsavel por iniciar os programas que serao escalonados    *
 * utilizando execve e enviar um sinal para eles entrarem em espera SIGSTOP *
 * Parametros:                                                              *
 * quantidadeProgramas - quantidade de programas sendo gerenciados          *
 * pid - vetor responsavel por retornar o pid dos programas                 *
 ****************************************************************************/
static void iniciarProgramas(int quantidadeProgramas, int *pid){

	int loop = 0;
	int pid_auxiliar = 0; //Variaveis responsaveis por guardar os pid dos processos

	/* Executa os programas e envia o sinal para entrarem em espera */
	for(loop=0;loop<quantidadeProgramas;loop++){

		pid_auxiliar = fork();

		if(pid_auxiliar!=0){

			pid[loop] = pid_auxiliar;
			kill(pid_auxiliar, SIGSTOP);
		}
		else{
			sleep(3); //Programas iniciados com uma diferenca de 3 segundos entre cada
			if(metodoEscalonamento == 1){
				execve(progPrioridade[loop]->nome,NULL,NULL);
			}
			else if(metodoEscalonamento == 2){
				execve(progRoundRobin[loop]->nome,NULL,NULL);
			}
			else if(metodoEscalonamento == 3){
				execve(progLoteria[loop]->nome,NULL,NULL);
			}
			else{
				printf("Problema com a identificacao do metodo de escalonamento na funcao iniciarProgramas\nPor favor, reinicie o programa.\n");
				exit(1);
			}
		}
	}
	/* FIM: Executa os programas e envia o sinal para entrarem em espera */
}

/****************************************************************************
 * Nome: testaProgramasFinalizados                                          *
 * Descricao: responsavel por testar se todos os programas ja foram         *
 * finalizados.                                                             *
 * Parametros:                                                              *
 * quantidadeProgramas - quantidade de programas sendo gerenciados          *
 * Condicoes de retorno:                                                    *
 * true - todos os programas ja foram finalizados                           *
 * false - ainda faltam programas a serem finalizados                       *
 ****************************************************************************/
static bool testaProgramasFinalizados(int quantidadeProgramas){

	int loop = 0;
	int contadorTerminados = 0;

	/**********************************************************************
	//Bloco de teste:       
	// printf("Teste de alcance para funcao testaProgramasFinalizados\n");
	//
	// printf("Lista de programas:\n");
	// for(loop=0;loop<quantidadeProgramas;loop++)
	// 	printf("%s\n", progRoundRobin[loop]->nome);
	/**********************************************************************/

	progRoundRobin[0]->terminado = true;

	for(loop=0;loop<quantidadeProgramas;loop++){
		
		if(metodoEscalonamento == 1){
			if(progPrioridade[loop]->terminado == false){
				contadorTerminados++;
			}
		}
		if(metodoEscalonamento == 2){
			if(progRoundRobin[loop]->terminado == false){
				contadorTerminados++;
			}
		}
		if(metodoEscalonamento == 3){
			if(progLoteria[loop]->terminado == false){
				contadorTerminados++;
			}
		}
	}

	if(contadorTerminados == quantidadeProgramas){
		//Teste	printf("Todos terminados\n");
		return true; // Retorna aviso de que todos os programas foram finalizados
	}

	/* TESTE
	for(loop=0;loop<quantidadeProgramas;loop++)
		if(progRoundRobin[loop]->terminado == false)
			printf("Falta %s\n", progRoundRobin[loop]->nome);
	*/

	return false; //Retorna aviso de que ainda faltam programas a serem finalizados
}