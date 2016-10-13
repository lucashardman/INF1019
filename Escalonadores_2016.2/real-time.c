#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

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

//função que recebe as informações sobre um programa e, se possível, o adiciona a lista de execução. Supoe 0 < I/D < 60
void adiciona(int signo)
{
	char nome[TAM];
	int I, D;
	int i;
	FILE *pArq; 
	if ((pArq = fopen("fifo", "r")) == NULL) { 
		puts ("Erro ao abrir a FIFO para escrita"); 
		exit(1); 
	} 
	
	if(fscanf("%s %d %d", nome, &I, &D) != 3)
	{
		exit(1);
	}
	
	//se I+D > 60, retorna
	if(I+D > 60)
	{
		return;
	}
	
	for(i = 0; i < qtd, I < lista[i].I; i++)
	//para cada elemento na lista cujo I é menor que o I obtido,
	{
		//se o intervalo desejado(I+D) coincidade com o do programa[i], retorna.
		if((I + D) < (lista[i].I + lista[i].D))
		{
			return;
		}
	}
	
	//sabendo que o intervalo desejado é compatível, adiciona ele na lista, e a reordena
	lista[qtd] = (Programa*) malloc(sizeof(Programa));
	lista[qtd].I = I;
	lista[qtd].D = D;
	strcpy(lista[qtd].nome, nome);
	//inicia execução do programa e o interrompe
	if(lista[qtd].pid = fork() < 0)
	{
		printf("Erro no fork\n");
		exit(1);
	}
	if(lista[qtd].pid == 0)
	{
		sleep()
	}
	
	
	
	
	
	
	
	fclose (pArq); 
	
	return;
}

//função chamada para encerrar o escalonador
void encerra(int signo)
{
	exit(1);
}

int main (void) { 
	
	signal(SIGUSR1, adiciona);
	signal(SIGUSR2, encerra);
	
	while(1);
	
	
	return 0; 
} 
