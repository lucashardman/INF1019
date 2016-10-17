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
	char nome[TAM], chamada[TAM+2] = "./";
	int I, D, pid;
	int i;
	
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
	strcpy(lista[qtd].nome, nome);
	
	//vetor programação
	
	for(i = I; i < I+D; i++)
	{
		programacao[i] = qtd;
		
	}	
	
	strcat(chamada, nome);
	
	//inicia execução do programa e o interrompe
	
	if((pid = fork()) < 0)
	{
		printf("Erro no fork\n");
		exit(1);
	}
	if(pid == 0)
	{
		
		execve(chamada, NULL, NULL);
	}
	else
	{
		lista[qtd].pid = pid;
	}	
	
	kill(pid, SIGSTOP);	
	
	qtd++;
			
	return;
}

//função chamada para fechar a fifo para leitura, chamada através de um sinal do interpretador quando não há mais programas a serem escalonados
void encerra(int signo)
{
	close(fpFIFO);
	return;
}

//função usada para remover um programa encerrado da lista de execução, liberando seu tempo de uso. recebe p que é a posição do programa no vetor de programas
void trata_fim(int p)
{
	int i;
	
	//remove p do vetor programação, e decrementa os outros programas de acordo

	for(i = 0; i < 60; i++)
	{
		if(programacao[i] == p)
		{
			programacao[i] = -1;
		}
		else if(programacao[i] > p)
		{
			programacao[i] = programacao[i] - 1;
		}
	}
	
	//realoca lista sem o programa
	for(i = p; i < qtd - 1;i++)
	{
		lista[i] = lista[i+1];
	}
	
	lista = (Programa*) realloc(lista, (qtd-1)*sizeof(Programa));

	// FIM: realoca lista sem o programa
	
	qtd--;
	
	return;
}

int main (void) { 
	
	int i, fd;
	int retwait;
	int status; //usado para obter status de programas através de waitpids
	int corrente; //o programa sendo executado no momento. -1 se não há.
	time_t s; // o segundo corrente em cada loop
	time_t inicio; //o segundo em que o escalonador começa sua execução
	
	signal(SIGUSR1, adiciona);
	signal(SIGUSR2, encerra);
	
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
	
	//altera saída padrão
	fd = open("saida.txt",O_CREAT|O_RDWR|O_TRUNC,0666);
	
	dup2(fd, 1);
	
	corrente = -1;
	inicio = time(NULL);
	
	while(1)
	{
		
		if((time(NULL) - inicio) > 60 && lista == NULL) break;
		s = (time(NULL) - inicio)%60;
		
		if(corrente != -1 && corrente != programacao[s])
		//se no segundo um programa está executando, mas não deveria
		{
			//se o programa encerrou por si próprio, trata isso
			//essa detecção tá dando ruim
			retwait = waitpid(lista[corrente].pid, &status, WNOHANG);
			if(retwait != 0 && WIFEXITED(status))
			{
				trata_fim(corrente);
				corrente = -1;
			}
			else
			{
				//pára o programa
				kill(lista[corrente].pid, SIGSTOP);
				corrente = -1;
			}
		}
	
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
				
				
				//prossegue execução do programa
				
				kill(lista[programacao[s]].pid, SIGCONT);
				corrente = programacao[s];
				//dorme até próximo do fim do tempo do programa
				sleep(lista[programacao[s]].D - 1);
			
			}
			else
			{
				sleep(0.25);
				continue;
			}
		}
		else
		{
			sleep(1);
			continue;
		}			
	
	}
	close(fd);
	return 0; 
} 
