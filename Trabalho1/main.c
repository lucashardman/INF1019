#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define maximo_programas 15 //Maximo de programas que podem ser executados simultaneamente

struct programaPrioridade{
	char execStr[5];
	char nome[15];
	int prioridade;
};

struct programaRoundRobin{
	char execStr[5];
	char nome[15];
};

struct programaLoteria{
	char execStr[5];
	char nome[15];
	int numeroTickets;
};

int main (void){

	FILE *exec;

	int metodoEscalonamento = 0; //1 - Prioridade - 2 - Round-robin - 3 - Lottery
	int loop = 0; //Variaveis auxiliares
	int contadorProgramas = 0; //Guarda o numero de programas em execucao

	struct programaPrioridade *lstProgramasPrioridade[maximo_programas]; //Lista de programas em execucao
	struct programaRoundRobin *lstProgramasRoundRobin[maximo_programas]; //Lista de programas em execucao
	struct programaLoteria *lstProgramasLoteria[maximo_programas]; //Lista de programas em execucao
	
	while(loop == 0){
		printf("Escolha a politica de escalonamento:\n1 - Por prioridade\n2 - Round-robin\n3 - Lottery\n");

		scanf("%d", &metodoEscalonamento);
		
		loop = 1;

		if(metodoEscalonamento != 1 && metodoEscalonamento != 2 && metodoEscalonamento != 3){
			printf("Politica de escalonamento invalida. Por favor, digite 1, 2 ou 3.\n");
			loop = 0;
		}
	}
	/****************************************************************************
	 * Preparacao dos programas a serem executados 								*
	 * 1 - Leitura do arquivo com a ordem de execucao dos programas				*
	 * 2 - Tratamento de erro para leitura do arquivo							*
	 * 3 - Alocacao de memoria para a estrutura que ira guardar os dados lidos	*
	 * 4 - Estrutura recebe os dados											*
	 ****************************************************************************/

	exec = fopen("exec.txt", "r");

	if (exec == NULL){
		printf("Erro na abertura do arquivo exec.exe");
		system("pause");
		exit(0);
	}

	/* Alocacao de memoria */
	for (loop=0; loop<maximo_programas; loop++){
		if(metodoEscalonamento == 1)
			lstProgramasPrioridade[loop] = (struct programaPrioridade*) malloc (sizeof(struct programaPrioridade) * 25);
		if(metodoEscalonamento == 2)
			lstProgramasRoundRobin[loop] = (struct programaRoundRobin*) malloc (sizeof(struct programaRoundRobin) * 25);
		if(metodoEscalonamento == 3)
			lstProgramasLoteria[loop] = (struct programaLoteria*) malloc (sizeof(struct programaLoteria) * 25);
	}
	/* Fim da alocacao de memoria */

	/* Estruturas recebem os dados do arquivo */
	if(metodoEscalonamento == 1){ //Para a politica de escalonamento POR PRIORIDADE
		//while(fscanf(exec, "%s %s %d", execStr, programa, &prioridade) == 3){ 
		while(fscanf(exec, "%s %s %d", lstProgramasPrioridade[contadorProgramas].execStr, lstProgramasPrioridade[contadorProgramas].nome, lstProgramasPrioridade[contadorProgramas]->prioridade) == 3){ 
			//strcpy(lstProgramasPrioridade[contadorProgramas]->nome, programa);
			contadorProgramas++;
		}
	}
	else if(metodoEscalonamento == 2){ //Para a politica de escalonamento ROUND-ROBIN
		//while(fscanf(exec, "%s %s", execStr, programa) != EOF){
		//	strcpy(lstProgramas[contadorProgramas], programa);
		//	contadorProgramas++;
		//}
	}
	else if(metodoEscalonamento == 3){ //Para a politica de escalonamento LOTTERY
		//while(fscanf(exec, "%s %s %d", execStr, programa, &numeroTickets) != EOF){
		//	strcpy(lstProgramas[contadorProgramas], programa);
		//	contadorProgramas++;
		//}
	}
	/* Fim: estruturas recebem os dados do arquivo */

	/****************************************************************************/
	//printf("Marcador 1\n");
	printf("%d\n",contadorProgramas);
	/* Teste politica 1:														*/
	//for(loop=0;loop<contadorProgramas;loop++)
	//	printf("1: %s\n",lstProgramasPrioridade[loop]->nome);
	/* Teste politica 2:														*/
	//while(loop<contadorProgramas)
	//	printf("%s\n",lstProgramas[loop]);
	/* Teste politica 3:														*/
	//while(loop<contadorProgramas)
	//	printf("%s\n",lstProgramas[loop]);
	/****************************************************************************/


	/* Fim da preparacao dos programas a serem executados  */

	if(metodoEscalonamento == 1){
	}
	else if(metodoEscalonamento == 2){
	}
	else if(metodoEscalonamento == 3){
	}
	else{
		printf("Numero invalido.\n");
	}

	fclose(exec); //Fecha o arquivo exec.exe

	return 0;
}
