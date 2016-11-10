#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_THREADS 5 
#define PULO_MAXIMO 100 
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
	pthread_mutex_lock(&lock); 
	printf("Sapo %d  chegou na posicaoo %d com %d pulos\n", (int) sapo, classificacao, pulos); 
	cont++; 
	classificacao++; 
	pthread_mutex_unlock(&lock);
	pthread_exit(NULL); 
} 

/*

Utilizando a função pthread_mutex_lock(&lock) restringir o acesso à região crítica, que é quando o sapo chega na posição.
Quando o sapo chegar na posição, os contadores cont e classificacao são incrementados, e esta é a parte mais importante que
não pode ser acessada pelos outros sapos (threads). Quando uma thread acabar de executar os incrementadores, não há mais 
necessidade de permanecer na região crítica, por isso utilizamos pthread_mutex_unlock(&lock) para ela sair e liberar a região.

[pirai.grad.inf.puc-rio.br:~/Documents/inf1019/Lab8] ./ex2
Corrida iniciada ... 
Sapo 1 pulou
Sapo 2 pulou
Sapo 0 pulou
Sapo 0 pulou
Sapo 1 pulou
Sapo 0  chegou na posicaoo 1 com 2 pulos
Sapo 2 pulou
Sapo 3 pulou
Sapo 1  chegou na posicaoo 2 com 2 pulos
Sapo 3 pulou
Sapo 3 pulou
Sapo 4 pulou
Sapo 4 pulou
Sapo 2 pulou
Sapo 3  chegou na posicaoo 3 com 3 pulos
Sapo 4  chegou na posicaoo 4 com 2 pulos
Sapo 2  chegou na posicaoo 5 com 3 pulos

 Acabou!!
[pirai.grad.inf.puc-rio.br:~/Documents/inf1019/Lab8] ./ex2
Corrida iniciada ... 
Sapo 1 pulou
Sapo 1 pulou
Sapo 1  chegou na posicaoo 1 com 2 pulos
Sapo 2 pulou
Sapo 2 pulou
Sapo 2  chegou na posicaoo 2 com 2 pulos
Sapo 4 pulou
Sapo 4 pulou
Sapo 4 pulou
Sapo 4  chegou na posicaoo 3 com 3 pulos
Sapo 0 pulou
Sapo 0 pulou
Sapo 0  chegou na posicaoo 4 com 2 pulos
Sapo 3 pulou
Sapo 3 pulou
Sapo 3 pulou
Sapo 3  chegou na posicaoo 5 com 3 pulos

 Acabou!!
[pirai.grad.inf.puc-rio.br:~/Documents/inf1019/Lab8] ./ex2
Corrida iniciada ... 
Sapo 1 pulou
Sapo 0 pulou
Sapo 2 pulou
Sapo 0 pulou
Sapo 1 pulou
Sapo 1  chegou na posicaoo 1 com 2 pulos
Sapo 2 pulou
Sapo 2  chegou na posicaoo 2 com 2 pulos
Sapo 0  chegou na posicaoo 3 com 2 pulos
Sapo 3 pulou
Sapo 3 pulou
Sapo 3  chegou na posicaoo 4 com 2 pulos
Sapo 4 pulou
Sapo 4 pulou
Sapo 4  chegou na posicaoo 5 com 2 pulos

 Acabou!!
[pirai.grad.inf.puc-rio.br:~/Documents/inf1019/Lab8] ./ex2
Corrida iniciada ... 
Sapo 0 pulou
Sapo 1 pulou
Sapo 2 pulou
Sapo 3 pulou
Sapo 0 pulou
Sapo 0  chegou na posicaoo 1 com 2 pulos
Sapo 2 pulou
Sapo 2  chegou na posicaoo 2 com 2 pulos
Sapo 3 pulou
Sapo 3  chegou na posicaoo 3 com 2 pulos
Sapo 1 pulou
Sapo 1  chegou na posicaoo 4 com 2 pulos
Sapo 4 pulou
Sapo 4 pulou
Sapo 4  chegou na posicaoo 5 com 2 pulos

 Acabou!!


*/
