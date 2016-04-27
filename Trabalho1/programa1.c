#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/* Programa I/O-BOUND - output: printf com o tempo que falta para terminar */

int main (void){

	int tempo = 14;

	printf("Este programa dura %d segundos.\n", tempo+1);

	while(tempo > 0){
		printf("Programa1 rodando. Faltam %d segundos.\n", tempo);
		sleep(1);
		tempo--;
	}
	
	printf("Programa1 finalizado.\n");

	return 0;
}
