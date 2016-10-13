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
	
	char str1[TAM], nome[TAM]; //buffers para o "exec" e o nome do programa
	//buffer para a prioridade
	int prioridade; 
	//buffer para os tempos e duração de execução
	int tempo, duracao;
	//buffer para char temporário
	char c;

	/* Inicializando FIFO, usado para comunicação entre interpretador e escalonador */
	int fpFIFO; 
	//se fifo não existe, cria fifo
	if(access("fifo", F_OK) == -1)
	{
		if(mkfifo("fifo", S_IRUSR | S_IWUSR) != 0)
		{
			printf("Erro ao criar fifo UP\n");
			return -1;
		}
	}
	//fim: se fifo não existe, cria fifo
	
	//abre fifo para escrita
	if((fpFIFO = open("fifo", O_WRONLY)) < 0)
	{
		printf("erro ao abrir fifo DOWN\n");
		return -2;
	}
	//fim: abre fifo para escrita
	
	//FIM: Inicializando FIFO, usado para comunicação entre interpretador e escalonador


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
		//inicia execução do escalonador prioridades
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
			write(fpFIFO, nome, strlen(nome));
			write(fpFIFO, &prioridade, sizeof(int)); 
			fflush(NULL);
			kill(pid, SIGUSR1);
			
			//espera 1 segundo entre envios dos programas
			sleep(1);
			
			//fecha e reabre a pipe para limpá-la
			/*
			fclose(pArq);
			if ((pArq = fopen("fifo", "w")) == NULL) { 
				puts ("Erro ao abrir a FIFO para escrita"); 
				return -1; 
			} */
			
		}
		//fputs("\n",pArq); // Finalizar arquivo
		kill(pid, SIGUSR2); //encerra o escalonador. Se quiser que continue, comentar.
	
	}
	else if(metodoEscalonamento == 2){
		//Round Robin
	}
	else if(metodoEscalonamento == 3){
		//REAL-TIME
		//inicia execução do escalonador real-time
		/*
		pid = fork();
		if(pid == 0)
		{
			execve("./real-time", NULL, NULL);	
		}
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
			//imprime as informações necessárias, a avisa o escalonador através de um sinal que há novos programas
			fprintf(pArq,"%s %d %d\n", nome , tempo, duracao);
			fflush(NULL);
			kill(pid, SIGUSR1);
			
			//espera 1 segundo entre envios dos programas
			sleep(1);
			
			//fecha e reabre a pipe
			fclose(pArq);
			if ((pArq = fopen("fifo", "w")) == NULL) { 
				puts ("Erro ao abrir a FIFO para escrita"); 
				return -1; 
			} 
			
		}
		fputs("\n",pArq); // Finalizar arquivo
		kill(pid, SIGUSR2); //encerra o escalonador. Se quiser que continue, comentar.
		*/
	}
	else{
		printf("Erro na chamada das funcoes de escalonamento: metodo invalido\n");
	}
	/* Fim: chamada das funcoes de escalonamento */


	//fclose (pArq); //fecha o arquivo fifo
	fclose(fpFIFO);
	fclose(exec); //Fecha o arquivo exec.exe
	/* Fim: Limpeza de memora e encerramento de arquivos */

	return 0;
}
