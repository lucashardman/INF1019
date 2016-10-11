#include "escalonadores.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

//tamanho máximo do nome de programa
#define TAM 10

int main (void){

	FILE *exec;
	int i = 0;
	int pid;
	int metodoEscalonamento = 0; //1 - Prioridade - 2 - Round-robin - 3 - Lottery
	int loop = 0; //Variaveis auxiliares
	int contadorProgramas = 0; //Guarda o numero de programas em execucao
	
	char str1[TAM], nome[TAM]; //buffers para o "exec", o nome do programa e a prioridade
	int prioridade; 

	ProgramaPrioridade *lstProgramasPrioridade[maximo_programas]; //Lista de programas em execucao
	ProgramaRoundRobin *lstProgramasRoundRobin[maximo_programas]; //Lista de programas em execucao
	ProgramaLoteria *lstProgramasLoteria[maximo_programas]; //Lista de programas em execucao

	/* Inicializando Pipe */
	FILE *pArq; 
	if ((pArq = fopen("fifo", "w")) == NULL) { 
		puts ("Erro ao abrir a FIFO para escrita"); 
		return -1; 
	} 


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
		printf("Erro na abertura do arquivo entrada.exe");
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
	
	/* Chamada das funcoes de escalonamento */
	
	if(metodoEscalonamento == 1){
		//inicia execução do escalonador
		pid = fork();
		if(pid == 0)
		{
			execve("./prioridades", NULL, NULL);	
		}
		sleep(3); //magia negra
		
		
		while((fscanf(exec, "%s %s %d", str1, nome, &prioridade) == 3)){ 
			if(prioridade < 1 || prioridade > 7){
				printf("Arquivo corrompido. Atualize o arquivo e reinicie o programa.\nPrioridade Minima: 1\nPrioridade Maxima: 7\n");
				exit(1);
			}
			//imprime as informações necessárias, a avisa o escalonador através de um sinal que há novos programas
			fprintf(pArq,"%s %d\n", nome ,prioridade);
			fflush(NULL);
			kill(pid, SIGUSR1);
			
			//espera 1 segundo entre envios dos programas
			sleep(1);
			
			fclose(pArq);
			if ((pArq = fopen("fifo", "w")) == NULL) { 
				puts ("Erro ao abrir a FIFO para escrita"); 
				return -1; 
			} 
			
		}
		fputs("\n",pArq); // Finalizar arquivo
	
	}
	else if(metodoEscalonamento == 2){
		//escalonamentoRoundRobin(contadorProgramas, lstProgramasRoundRobin);
	}
	else if(metodoEscalonamento == 3){
		//escalonamentoLoteria(contadorProgramas, lstProgramasLoteria);
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
	fclose (pArq);
	fclose(exec); //Fecha o arquivo exec.exe
	/* Fim: Limpeza de memora e encerramento de arquivos */

	return 0;
}
