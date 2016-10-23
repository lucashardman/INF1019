#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>

//tamanho máximo do nome. importante ser o mesmo definido em main.c
#define TAM 10

//Quantidade maxima de programas sendo escalonados
#define MAX_PROG 10

int fpFIFO;

//estrutura que armazena informações sobre um programa
typedef struct programa
{
	//nome do programa
	char nome[TAM];
	//pid do programa
	int pid;
	//true se o programa estiver terminado
	bool terminado;
} Programa;

Programa *lista;
int qtd=0;
int terminado=0;

void carrega_programa(int signo);
void encerra(int signo);

int main (int argc, char *argv[]) {
	int pid = 0, pidPai=0, status;
	int loop = 0, i=0, j=0;
	int ordem[MAX_PROG];

	time_t s; // o segundo corrente em cada loop
	time_t inicio; //o segundo em que o escalonador começa sua execução

	signal(SIGUSR1, carrega_programa);
	signal(SIGUSR2, encerra);

	//se fifo existe, abre fifo para leitura
	if(access("fifo", F_OK) == 0)
	{
		if((fpFIFO = open("fifo", O_RDONLY)) <= 0)
		{
			printf("erro ao abrir fifo\n");
			return -2;
		}
	}

	//FIM: se fifo existe, abre fifo para leitura
	while(1){

		for(loop=0;loop<qtd;loop++){ // Este loop eh apenas para garantir que os programas estao todos em estado de espera antes do escalonamento comecar.
			kill(lista[loop].pid, SIGSTOP);
		}

		fflush(stdout);	
		
		i=0;
		while(i<qtd){
			if(lista[i].terminado == false){
					
				printf("O programa %s vai recomecar.\n", lista[i].nome);

				if(waitpid(lista[i].pid, &status, WNOHANG) != 0){ //Waitpid != 0, se o processo com pid passado como argumento estiver terminado
					printf("--> Programa %s terminado\n", lista[i].nome);
					lista[i].terminado = true;
				}
				for(loop=0;loop<qtd;loop++){//Testa se todos os programas terminaram.
					if(lista[loop].terminado == true){
						terminado++;
					}
				}

				if(terminado == qtd){//Se todos programas terminaram, termina.
					printf("--> Todos os programas terminaram.\n");
					kill(pidPai, SIGUSR2);
					return 0;
				}

				terminado = 0;

				fflush(stdout);
				sleep(1);
				kill(lista[i].pid, SIGCONT); // O programa eh escalonado. Entra em estado de execucao.
				fflush(stdout);
				sleep(3); // Fatia de tempo de 3 segundos.
				//usleep(500); // Fatia de tempo de 0.5 segundos.
				kill(lista[i].pid, SIGSTOP); // O programa passa a vez. Entra em estado de espera.
			}
			i++;
		}
	}
	return 0;
}

/****************************************************************************
 * Carrega_programa                                                         *
 * 1 - Esta funcao eh um handler para o sinal SIGUSR1.                      *
 * 2 - Le a FIFO e utiliza os dado lidos para inicializar um programa e     *
 * coloca-lo em estado de espera.                                           *
 * 3 - Os pid sao armazenados   *
 * 4 - Estrutura recebe os dados                                            *
 ****************************************************************************/
void carrega_programa(int signo){

	int prioridade;
	char nome_aux[TAM];
	int pid, status;
	int pid_teste;
	read(fpFIFO, nome_aux, TAM);

	//sabendo que o intervalo desejado é compatível, adiciona ele na lista e no vetor programacao
	//lista
	lista = realloc(lista, (qtd+1)*sizeof(Programa));
	if(lista == NULL){
		printf("Erro de alocação de memória\n");
		exit(1);
	}

	pid = fork();

	strcpy(lista[qtd].nome,"");
	strcat(lista[qtd].nome, "./");
	strcat(lista[qtd].nome, nome_aux);
	
	if(pid!=0){
		lista[qtd].pid = pid;
	}
	else{
		lista[qtd].terminado = false;
		execve(lista[qtd].nome, NULL, NULL);
	}
	
	kill(lista[qtd].pid, SIGSTOP);

	printf("Programa %s de PID %d carregado.\n", lista[qtd].nome, lista[qtd].pid);
	fflush(stdout);
	qtd++;
}

//função chamada para fechar a fifo
void encerra(int signo){	
	close(fpFIFO); // O vetor ordem armazena a ordem de programas que estao sendo escalonados. 
	return;
}
