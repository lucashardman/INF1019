#include <stdio.h>
#include <stdlib.h>

int main (void){

	int metodoEscalonamento = 0;

	printf("Escolha a politica de escalonamento:\n1 - Por prioridade\n2 - Round-robin\n3 - Lottery\n");

	scanf("%d", &metodoEscalonamento);

	if(metodoEscalonamento == 1){
	}
	else if(metodoEscalonamento == 2){
	}
	else if(metodoEscalonamento == 3){
	}
	else{
		printf("Numero invalido.\n");
	}

	return 0;
}
