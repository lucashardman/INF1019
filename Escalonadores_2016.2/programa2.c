#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/* Programa I/O-BOUND - output: printf com o tempo que falta para terminar */

int main (void){

	int tempo = 19;

	printf("Este programa dura %d segundos.\n", tempo+1);

	while(tempo > 0){
		printf("Programa2 rodando. Faltam %d segundos.\n", tempo);
		sleep(1);
		tempo--;
	}
	
	printf("Programa2 finalizado.\n");

	return 0;
}
