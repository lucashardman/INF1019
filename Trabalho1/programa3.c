#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main (void){

	int i = 0, j = 0;

	while (j == 0){
		while(i < 10){
			printf("Programa3 rodando.\n");
			sleep(1);
			i++;
		}
		printf("Tecle qualquer numero diferente de 0 para continuar rodando: ");
		scanf("%d", &j);
		i = 0;
	}
	return 0;
}
