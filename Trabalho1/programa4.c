#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main (void){

	int tempo = 24;

	printf("Este programa dura %d segundos.\n", tempo+1);

	while(tempo > 0){
		printf("Programa4 rodando. Faltam %d segundos.\n", tempo);
		sleep(1);
		tempo--;
	}
	
	printf("Programa4 finalizado.\n");

	return 0;
}
