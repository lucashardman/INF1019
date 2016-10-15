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


//tamanho máximo para nome de programa
#define TAM 10

//estrutura que armazena informações sobre um programa
typedef struct programa
{
	//nome do programa
	char nome[TAM];
	//pid do programa
	int pid;
	//intervalo de início da execução de um programa
	int I;
	//duração da execução de um programa
	int D;
	//true se já encerrou, false se não
	bool terminado;
} Programa;

//vetor de programas a serem administrados e quantidade
int qtd = 0;
Programa *lista;

//vetor de 60 posições que armazena o índice no vetor de programa do programa que deve estar executando naquele segundo.

int programacao[60];

//identificador da fifo

int fpFIFO;

//função que recebe as informações sobre um programa e, se possível, o adiciona a lista de execução. Supoe 0 < I/D < 60
void adiciona(int signo)
{
	char nome[TAM];
	int I, D, pid;
	int i;
	
	printf("lendo fifo\n");
	
	read(fpFIFO, nome, TAM);
	read(fpFIFO, &I, sizeof(int));
	read(fpFIFO, &D, sizeof(int));
	
	
	//se I+D > 60, retorna, já que o intervalo não pode ser comportado
	if(I+D > 60)
	{
		return;
	}
	
	//checa se o intervalor desejado não conflita com outros já usado
	for(i = I; i < I+D; i++)
	//para cada segundo desejado,
	{
		
		//se o segundo não está disponível, retorna
		if(programacao[i] != -1) return;
	}
	
	
	//sabendo que o intervalo desejado é compatível, adiciona ele na lista e no vetor programacao
	//lista
	lista = realloc(lista, (qtd+1)*sizeof(Programa));
	if(lista == NULL)
	{
		printf("Erro de alocação de memória\n");
		exit(1);
	}
	//lista[qtd] = (Programa) malloc(sizeof(Programa));
	lista[qtd].I = I;
	lista[qtd].D = D;
	lista[qtd].terminado = false;
	strcpy(lista[qtd].nome, nome);
	
	//vetor programação
	
	for(i = I; i < I+D; i++)
	{
		programacao[i] = qtd;
		
	}	
	
	
	//inicia execução do programa e o interrompe
	
	if((pid = fork()) < 0)
	{
		printf("Erro no fork\n");
		exit(1);
	}
	if(pid == 0)
	{
		printf("fork: %d\n", getpid());
		execve("./programa1", NULL, NULL);
	}
	else
	{
		lista[qtd].pid = pid;
	}	
	
	kill(lista[qtd].pid, SIGSTOP);	
	
	qtd++;
	
	printf("aqui!\n");
			
	return;
}

//função chamada para encerrar o escalonador
void encerra(int signo)
{
	printf("fechando fifo\n");
	close(fpFIFO);
	printf("fifo fechada\n");
	return;
}

int main (void) { 
	
	int i;
	int corrente; //o programa sendo executado no momento. -1 se não há.
	time_t s; // o segundo corrente em cada loop
	time_t inicio; //o segundo em que o escalonador começa sua execução
	
	signal(SIGUSR1, adiciona);
	if((signal(SIGUSR2, encerra)) == SIG_ERR) printf("erro na signal\n");
	
	for(i = 0; i < 60; i++)
	{
		programacao[i] = -1;
	}
	
	
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
	
	
	corrente = -1;
	inicio = time(NULL);
	//AFAZER: tratar fim de programas
	while(1)
	{
		
		s = (time(NULL) - inicio)%60;
		printf("%d %d \n", s, programacao[s]);
		
		if(programacao[s] >= 0 && programacao[s] != corrente)
		//se um programa deveria estar sendo executado, mas não está, 
		{
			
			if(s >= lista[programacao[s]].I && s < (lista[programacao[s]].I + lista[programacao[s]].D))
			//redundância. se não coincidir, continue
			{
				if(corrente >= 0)
				//se já havia um programa em execução, o interrompe
				{
					kill(lista[corrente].pid, SIGSTOP);
				}
				//começa execução do novo programa corrente
				printf("começando execução de programa %d \n", lista[programacao[s]].pid);
				kill(lista[programacao[s]].pid, SIGCONT);
				corrente = programacao[s];
				//dorme até próximo do fim do tempo do programa
				//sleep(lista[programacao[s]].D - 1);
			}
			else
			{
				sleep(0.25);
				continue;
			}
		}
		else
		//
		{
			if(programacao[s] == -1 && corrente >= 0)
			//se o segundo deveria estar livre mas há um programa executando,
			{
				//pára o programa
				printf("parando %d\n", lista[corrente].pid);
				kill(lista[corrente].pid, SIGSTOP);
				corrente = -1;
			}
			else
			{
				sleep(1);
				continue;
			}
		}
		
	
	}
	
	return 0; 
} 
