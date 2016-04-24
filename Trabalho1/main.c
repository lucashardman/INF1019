#include "escalonadores.h"

int main (void){

	FILE *exec;

	int metodoEscalonamento = 0; //1 - Prioridade - 2 - Round-robin - 3 - Lottery
	int loop = 0; //Variaveis auxiliares
	int contadorProgramas = 0; //Guarda o numero de programas em execucao

	ProgramaPrioridade *lstProgramasPrioridade[maximo_programas]; //Lista de programas em execucao
	ProgramaRoundRobin *lstProgramasRoundRobin[maximo_programas]; //Lista de programas em execucao
	ProgramaLoteria *lstProgramasLoteria[maximo_programas]; //Lista de programas em execucao

	/* Escolha da politica de escalonamento */
	while(loop == 0){
		printf("Escolha a politica de escalonamento:\n1 - Por prioridade\n2 - Round-robin\n3 - Loteria\n");

		scanf("%d", &metodoEscalonamento);
		
		loop = 1;

		if(metodoEscalonamento != 1 && metodoEscalonamento != 2 && metodoEscalonamento != 3){
			printf("Politica de escalonamento invalida. Por favor, digite 1, 2 ou 3.\n");
			loop = 0;
		}
	}
	/* Fim: Escolha da politica de escalonamento */

	/****************************************************************************
	 * Preparacao dos programas a serem executados                              *
	 * 1 - Leitura do arquivo com a ordem de execucao dos programas             *
	 * 2 - Tratamento de erro para leitura do arquivo                           *
	 * 3 - Alocacao de memoria para a estrutura que ira guardar os dados lidos  *
	 * 4 - Estrutura recebe os dados                                            *
	 ****************************************************************************/

	exec = fopen("entrada.txt", "r");

	if (exec == NULL){
		printf("Erro na abertura do arquivo exec.exe");
		exit(0);
	}

	/* Alocacao de memoria */
	for (loop=0; loop<maximo_programas; loop++){
		if(metodoEscalonamento == 1)
			lstProgramasPrioridade[loop] = (ProgramaPrioridade*) malloc (sizeof(ProgramaPrioridade) * 25);
		if(metodoEscalonamento == 2)
			lstProgramasRoundRobin[loop] = (ProgramaRoundRobin*) malloc (sizeof(ProgramaRoundRobin) * 25);
		if(metodoEscalonamento == 3)
			lstProgramasLoteria[loop] = (ProgramaLoteria*) malloc (sizeof(ProgramaLoteria) * 25);
	}
	/* Fim da alocacao de memoria */

	/* Estruturas recebem os dados do arquivo */
	if(metodoEscalonamento == 1){ //Para a politica de escalonamento POR PRIORIDADE
		while(fscanf(exec, "%s %s %d", (lstProgramasPrioridade[contadorProgramas])->execStr, (lstProgramasPrioridade[contadorProgramas])->nome, &(lstProgramasPrioridade[contadorProgramas])->prioridade) == 3){ 
			if((lstProgramasPrioridade[contadorProgramas])->prioridade < 1 || (lstProgramasPrioridade[contadorProgramas])->prioridade > 7){
				printf("Arquivo corrompido. Atualize o arquivo e reinicie o programa.\nPrioridade Minima: 1\nPrioridade Maxima: 7\n");
				exit(1);
			}
			contadorProgramas++;
		}
	}
	else if(metodoEscalonamento == 2){ //Para a politica de escalonamento ROUND-ROBIN
		while(fscanf(exec, "%s %s", (lstProgramasRoundRobin[contadorProgramas])->execStr, (lstProgramasRoundRobin[contadorProgramas])->nome) == 2){ 
			contadorProgramas++;
		}
	}
	else if(metodoEscalonamento == 3){ //Para a politica de escalonamento LOTTERY
		while(fscanf(exec, "%s %s %d", (lstProgramasLoteria[contadorProgramas])->execStr, (lstProgramasLoteria[contadorProgramas])->nome, &(lstProgramasLoteria[contadorProgramas])->quantidadeBilhetes) == 3){ 
			contadorProgramas++;
		}
	}
	/* Fim: estruturas recebem os dados do arquivo */

	/****************************************************************************/
	// BLOCO DE TESTE:      
	//                                                    
	//printf("Marcador 1\n");
	//printf("%d\n",contadorProgramas);
	//
	// Teste politica 1:														
	//for(loop=0;loop<contadorProgramas;loop++)
	//	printf("1: %s - prioridade: %d\n",lstProgramasPrioridade[loop]->nome, lstProgramasPrioridade[loop]->prioridade);
	//
	// Teste politica 2:														
	//for(loop=0;loop<contadorProgramas;loop++)
	//	printf("1: %s\n",lstProgramasRoundRobin[loop]->nome);
	//
	// Teste politica 3:														
	//for(loop=0;loop<contadorProgramas;loop++)
	//	printf("3: %s - numero de tickets: %d\n",lstProgramasLoteria[loop]->nome, lstProgramasLoteria[loop]->numeroTickets);
	/****************************************************************************/

	/* Chamada das funcoes de escalonamento */
	if(metodoEscalonamento == 1){
		escalonamentoPorPrioridade(contadorProgramas, lstProgramasPrioridade);
	}
	else if(metodoEscalonamento == 2){
		escalonamentoRoundRobin(contadorProgramas, lstProgramasRoundRobin);
	}
	else if(metodoEscalonamento == 3){
		escalonamentoLoteria(contadorProgramas, lstProgramasLoteria);
	}
	else{
		printf("Erro na chamada das funcoes de escalonamento: metodo invalido\n");
	}
	/* Fim: chamada das funcoes de escalonamento */

	/* Limpeza de memora e encerramento de arquivos */
	for(loop=0;loop<maximo_programas;loop++){

		if(metodoEscalonamento == 1)
			free(lstProgramasPrioridade[loop]);
		if(metodoEscalonamento == 2)
			free(lstProgramasRoundRobin[loop]);
		if(metodoEscalonamento == 3)
			free(lstProgramasLoteria[loop]);
	}
	fclose(exec); //Fecha o arquivo exec.exe
	/* Fim: Limpeza de memora e encerramento de arquivos */

	return 0;
}
