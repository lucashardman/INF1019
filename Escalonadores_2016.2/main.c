#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>

//tamanho máximo do nome de programa
#define TAM 10

int main (void){

	FILE *exec;
	int pid;
	int metodoEscalonamento = 0; //1 - Prioridade - 2 - Round-robin - 3 - Real-Time
	int loop = 0; //Variaveis auxiliares
	char strPid[TAM];
	char *argv[3];
	char str1[TAM], nome[TAM]; //buffers para o "exec" e o nome do programa
	//buffer para a prioridade
	int prioridade; 
	//buffer para os tempos e duração de execução
	int tempo, duracao;
	//buffer para char temporário
	char c;

	int fpFIFO; 
	//se fifo não existe, cria fifo
	if(access("fifo", F_OK) == -1)
	{
		if(mkfifo("fifo", S_IRUSR | S_IWUSR) != 0)
		{
			printf("Erro ao criar fifo\n");
			return -1;
		}
	}
	//FIM: se fifo não existe, cria fifo


//AFAZER: alterar saída padrão para arquivo

	/* Escolha da politica de escalonamento */
	while(loop == 0){
		printf("Escolha a politica de escalonamento:\n1 - Por prioridade\n2 - Round-robin\n3 - Real-Time\n");

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
	

	
	/* Chamada das funcoes de escalonamento */
	
	if(metodoEscalonamento == 1){
		// PRIORIDADES

		sprintf(strPid, "%d", getpid());

		//Passando o pid atraves do argv para matar o processo pai no final
		argv[0]="./Prioridades";
		argv[1]=strPid;
		argv[2]=NULL;

		//inicia execução do escalonador prioridades
		pid = fork();
		if(pid == 0)
		{
			execve("./prioridades", argv, NULL);	
		}
		
		//abre fifo para escrita
		
		if((fpFIFO = open("fifo", O_WRONLY)) < 0)
		{
			printf("erro ao abrir fifo.\n");
			return -2;
		}
		
		//fim: abre fifo para escrita
		
		sleep(3); //magia negra

	
		while((fscanf(exec, "%s %s %d", str1, nome, &prioridade) == 3))
		{ 	
			
			if(prioridade < 1 || prioridade > 7){
				printf("Arquivo corrompido. Atualize o arquivo e reinicie o programa.\nPrioridade Minima: 1\nPrioridade Maxima: 7\n");
				exit(1);
			}
			//manda as informações necessárias, a avisa o escalonador através de um sinal que há novos programas
			write(fpFIFO, nome, TAM);
			write(fpFIFO, &prioridade, sizeof(int)); 
			fflush(NULL);
			kill(pid, SIGUSR1);
			
			//espera 1 segundo entre envios dos programas
			sleep(1);			
		}
		kill(pid, SIGUSR2); //Fecha a fifo no lado do escalonador, sem interromper sua execução.
	
	}
	else if(metodoEscalonamento == 2){
		//Round Robin
	}
	else if(metodoEscalonamento == 3){
		//REAL-TIME
		//inicia execução do escalonador real-time
		
		pid = fork();
		if(pid == 0)
		{
			execve("./real-time", NULL, NULL);	
		}
		
		
		//abre fifo para escrita
		
		if((fpFIFO = open("fifo", O_WRONLY)) < 0)
		{
			printf("erro ao abrir fifo.\n");
			return -2;
		}
		
		//fim: abre fifo para escrita
		
		sleep(3); //magia negra
		
		
		while((fscanf(exec, "%s %s %c %c %d %c %c %d", str1, nome, &c, &c, &tempo, &c, &c, &duracao) == 8)){ 
			if(tempo < 0 || tempo > 60){
				printf("Arquivo corrompido. Atualize o arquivo e reinicie o programa.\nI Minimo: 0\nI Maximo: 60\n");
				exit(1);
			}
			if(duracao < 0 || duracao > 60){
				printf("Arquivo corrompido. Atualize o arquivo e reinicie o programa.\nD Minimo: 0\nD Maximo: 60\n");
				exit(1);
			}
			
			//manda as informações necessárias, a avisa o escalonador através de um sinal que há novos programas
			
			write(fpFIFO, nome, TAM);
			write(fpFIFO, &tempo, sizeof(int)); 
			write(fpFIFO, &duracao, sizeof(int)); 
			fflush(NULL);
			kill(pid, SIGUSR1);
			
			
			//espera 1 segundo entre envios dos programas
			
			sleep(1);
			
			
		}
		printf("aqui!!!\n");
		kill(pid, SIGUSR2); //Fecha a fifo no lado do escalonador, sem interromper sua execução.
		
	}
	else{
		printf("Erro na chamada das funcoes de escalonamento: metodo invalido\n");
	}
	/* Fim: chamada das funcoes de escalonamento */
	
	close(fpFIFO); //fecha a fifo. 
	
	fclose(exec); //Fecha o arquivo exec.exe
	/* Fim: Limpeza de memora e encerramento de arquivos */
	
	//printf("encerrando main\n");
	//AFAZER: solucionar problema de terminar main terminar escalonador.
	while(1){

	}
	return 0; //tá encerrando o processo filho???
}
