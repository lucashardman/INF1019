#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/* Programa CPU-BOUND - Incrementa uma variavel chamada soma ate chegar em 24 */

int main (void){

	int soma = 0;

	while(soma < 24){
		sleep(1);
		soma++;
	}

	return 0;
}
