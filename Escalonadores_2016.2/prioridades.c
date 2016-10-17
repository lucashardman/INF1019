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
	//prioridade do programa
	int prioridade;
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
static int *ordemPrioridade(int quantidadeProgramas, int *ordem);

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
	
	
	printf("O PID EH %s\n", argv[1]);

	pidPai = atoi(argv[1]);

	printf("O PID INT EH %d\n", pidPai);	

	while(1){

		for(loop=0;loop<qtd;loop++){ // Este loop eh apenas para garantir que os programas estao todos em estado de espera antes do escalonamento comecar.
			kill(lista[loop].pid, SIGSTOP);
		}

		sleep(3);
		
		fflush(stdout);	
		/* O vetor ordem armazena a ordem de programas que estao sendo escalonados. Como a funcao ordemPrioridade esta sendo chamada 
		 * em todo o loop, o vetor ordem esta sendo atualizado em tempo real. Ou seja, quando um novo programa entra para o escalonamento,
		 * ele automaticamente eh colocado para executar na sua ordem certa.
		 * OBS: Apos um programa ser carregado, ele soh entra para o escalonamento no proximo loop. Por exemplo, temos 3 programas: x de prioridade 1
		 * y de prioridade 2 e z de prioridade 3. Se x estiver em execucao, z em estado de espera e y acabou de ser carregado, z vai ser o proximo
		 * programa a ser escalonado (mas no proximo loop, y sera escalonado entre x e z). */
		ordemPrioridade(qtd, ordem);
		
		//inicio = time(NULL);

		i=0;
		while(i<qtd){

			//s = (time(NULL) - inicio)%60;

			for(j=0;j<qtd;j++){
				/* Compara a prioridade de lista[j] com a ordem[i]. Se for a mesma, entao esta na vez do programa
				 * de lista[j] ser escalonado. */
				if(lista[j].prioridade == ordem[i] && lista[j].terminado == false){

					printf("%s de prioridade %d vai recomecar.\n", lista[j].nome, ordem[i]);

					if(waitpid(lista[j].pid, &status, WNOHANG) != 0){ //Waitpid != 0, se o processo com pid passado como argumento estiver terminado
						printf("--> Programa %s terminado\n", lista[j].nome);
						lista[j].terminado = true;
					}
					for(loop=0;loop<qtd;loop++){//Testa se todos os programas terminaram.
						if(lista[loop].terminado == true){
							terminado++;
						}
					}
					if(terminado == qtd){//Se todos programas terminaram, termina.
						printf("--> Todos os programas terminaram.\n");
						kill(pidPai, SIGKILL); //Mata o processo pai.
						return 0;
					}
					terminado = 0;

					fflush(stdout);
					sleep(1);
					kill(lista[j].pid, SIGCONT); // O programa eh escalonado. Entra em estado de execucao.
					fflush(stdout);
					sleep(3); // O programa executa por 3 segundos.
					kill(lista[j].pid, SIGSTOP); // O programa passa a vez. Entra em estado de espera.
				}
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
	read(fpFIFO, &prioridade, sizeof(int));

	//sabendo que o intervalo desejado é compatível, adiciona ele na lista e no vetor programacao
	//lista
	lista = realloc(lista, (qtd+1)*sizeof(Programa));
	if(lista == NULL){
		printf("Erro de alocação de memória\n");
		exit(1);
	}
	lista[qtd].prioridade = prioridade;

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

	printf("Programa %s de PID %d e prioridade %d carregado.\n", lista[qtd].nome, lista[qtd].pid, lista[qtd].prioridade);
	fflush(stdout);
	qtd++;
}

//função chamada para fechar a fifo
void encerra(int signo){	
	close(fpFIFO); // O vetor ordem armazena a ordem de programas que estao sendo escalonados. 
	return;
}

/****************************************************************************
 * Nome: ordemPrioridade                                                    *
 * Descricao: serve para informar a ordem de prioridade dos programas.      *
 * Utiliza um algoritimo de ordenacao por bolha (bubble sort) para ordenar  *
 * as prioridades dos programas.                                            *
 * Parametros:                                                              *
 * quantidadeProgramas - quantidade de programas sendo escalonados          *
 * Retorno:                                                                 *
 * ordem - vetor com a ordem de execucao dos programas. Cada posicao do     *
 * corresponde a prioridade de um programa                                  *
 ****************************************************************************/
static int *ordemPrioridade(int quantidadeProgramas, int *ordem){
	
	int loop1 = 0, loop2 = 0;
	//int *ordem;
	int auxiliar = 100;

	//ordem = (int*) malloc (sizeof(int)*quantidadeProgramas);

	for(loop1=0;loop1<quantidadeProgramas;loop1++){
		ordem[loop1] = lista[loop1].prioridade;
	}

	for(loop1=quantidadeProgramas-1;loop1>=1;loop1--){
		for(loop2=0;loop2<loop1;loop2++){
			if(ordem[loop2]>ordem[loop2+1]){
				auxiliar = ordem[loop2];
				ordem[loop2] = ordem[loop2+1];
				ordem[loop2+1] = auxiliar;
			}
		}
	}
	return ordem;
}