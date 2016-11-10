#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_THREADS      5 
#define PULO_MAXIMO      100 
#define DESCANSO_MAXIMO  1 
#define DISTANCIA_PARA_CORRER 100 

static int classificacao = 1; 
static pthread_mutex_t lock; 
static char * resp[200]; 
static int cont = 0;

void *Correr(void  *sapo);

int main() { 

	classificacao =1; 
	pthread_t threads[NUM_THREADS]; 
	int t; 
	
	printf("Corrida iniciada ... \n"); 
	for(t=0;t < NUM_THREADS;t++)
		pthread_create(&threads[t], NULL, Correr, (void *) t); 
	for(t=0;t < NUM_THREADS; t++)
		pthread_join(threads[t],NULL);
		
	printf("\n Acabou!!\n"); 
	pthread_exit(NULL); 
} 
void *Correr(void  *sapo){ 
	int pulos = 0; 
	int distanciaJaCorrida = 0; 
	while (distanciaJaCorrida <= DISTANCIA_PARA_CORRER) { 
		int pulo = rand() % PULO_MAXIMO; 
		distanciaJaCorrida += pulo; 
		pulos++; 
		printf("Sapo %d pulou\n", (int) sapo); 
		int descanso = rand() % DESCANSO_MAXIMO; 
		sleep(descanso); 
	} 
	
	printf("Sapo %d  chegou na posicaoo %d com %d pulos\n", (int) sapo, classificacao, pulos); 
	cont++; 
	classificacao++; 
	pthread_exit(NULL); 
} 

/*

Observando o output do programa em algumas execuções, vemos que em alguns casos mais de um sapo chega na mesma posição.
Isto acontece pois a posição deveria ser a região protegida e aceitar no máximo 1 sapo.

[pirai.grad.inf.puc-rio.br:~/Documents/inf1019/Lab8] ./ex1
Corrida iniciada ... 
Sapo 0 pulou
Sapo 1 pulou
Sapo 2 pulou
Sapo 2 pulou
Sapo 2  chegou na posicaoo 1 com 2 pulos
Sapo 0 pulou
Sapo 0  chegou na posicaoo 2 com 2 pulos
Sapo 1 pulou
Sapo 1  chegou na posicaoo 3 com 2 pulos
Sapo 3 pulou
Sapo 3 pulou
Sapo 3  chegou na posicaoo 4 com 2 pulos
Sapo 4 pulou
Sapo 4 pulou
Sapo 4  chegou na posicaoo 5 com 2 pulos

 Acabou!!
[pirai.grad.inf.puc-rio.br:~/Documents/inf1019/Lab8] ./ex1
Corrida iniciada ... 
Sapo 2 pulou
Sapo 1 pulou
Sapo 2 pulou
Sapo 1 pulou
Sapo 1  chegou na posicaoo 1 com 2 pulos
Sapo 3 pulou
Sapo 3 pulou
Sapo 3 pulou
Sapo 2  chegou na posicaoo 2 com 2 pulos
Sapo 3  chegou na posicaoo 2 com 3 pulos
Sapo 4 pulou
Sapo 4 pulou
Sapo 4 pulou
Sapo 4 pulou
Sapo 4  chegou na posicaoo 4 com 4 pulos
Sapo 0 pulou
Sapo 0 pulou
Sapo 0  chegou na posicaoo 5 com 2 pulos

 Acabou!!
[pirai.grad.inf.puc-rio.br:~/Documents/inf1019/Lab8] ./ex1
Corrida iniciada ... 
Sapo 0 pulou
Sapo 0 pulou
Sapo 0  chegou na posicaoo 1 com 2 pulos
Sapo 1 pulou
Sapo 1 pulou
Sapo 1  chegou na posicaoo 2 com 2 pulos
Sapo 2 pulou
Sapo 2 pulou
Sapo 2  chegou na posicaoo 3 com 2 pulos
Sapo 3 pulou
Sapo 3 pulou
Sapo 3  chegou na posicaoo 4 com 2 pulos
Sapo 4 pulou
Sapo 4 pulou
Sapo 4  chegou na posicaoo 5 com 2 pulos

 Acabou!!
[pirai.grad.inf.puc-rio.br:~/Documents/inf1019/Lab8] ./ex1
Corrida iniciada ... 
Sapo 0 pulou
Sapo 2 pulou
Sapo 1 pulou
Sapo 1 pulou
Sapo 0 pulou
Sapo 2 pulou
Sapo 1  chegou na posicaoo 1 com 2 pulos
Sapo 0  chegou na posicaoo 1 com 2 pulos
Sapo 3 pulou
Sapo 3 pulou
Sapo 3 pulou
Sapo 3  chegou na posicaoo 3 com 3 pulos
Sapo 2 pulou
Sapo 2  chegou na posicaoo 4 com 3 pulos
Sapo 4 pulou
Sapo 4 pulou
Sapo 4 pulou
Sapo 4  chegou na posicaoo 5 com 3 pulos

 Acabou!!


*/
