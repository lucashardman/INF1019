#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

void imprime(int signo)
{
	char ch;
	FILE *pArq; 
	if ((pArq = fopen("fifo", "r")) == NULL) { 
		puts ("Erro ao abrir a FIFO para escrita"); 
		exit(1); 
	} 
	
	while ((ch = fgetc(pArq)) != EOF) putchar (ch); 
	
	fclose (pArq); 
	
	//printf("asdasd\n");
	return;
}

int main (void) { 
	
	signal(SIGUSR1, imprime);
	while(1){
		//printf("NAO\n");
	}
	
	
	return 0; 
} 
