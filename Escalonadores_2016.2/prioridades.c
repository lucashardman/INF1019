#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

//tamanho máximo do nome. importante ser o mesmo definido em main.c
#define TAM 10


int fpFIFO;

void imprime(int signo)
{
	int prioridade;
	char nome[TAM];

	read(fpFIFO, nome, TAM);
	read(fpFIFO, &prioridade, sizeof(int));
	printf("%s %d\n", nome, prioridade);
	fflush(stdout);
	return;
}

//função chamada para fechar a fifo
void encerra(int signo)
{	
	close(fpFIFO);
	//provisório exit(1)
	exit(1);
	return;
}

int main (void) { 
	
	signal(SIGUSR1, imprime);
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
	
	
	while(1)
	{
		
	}	
	
	
	return 0; 
} 
