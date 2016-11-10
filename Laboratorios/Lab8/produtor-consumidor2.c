#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define MAXFILA 8
#define MAXRAND 100

#define NP 1
#define NC 3

//buffer e contador de elementos no buffer
static int buffer[MAXFILA];
static int n = 0;
//mutex e variável de condição
static pthread_mutex_t lock;
static pthread_mutex_t mutex1;
static pthread_mutex_t mutex2; 

pthread_cond_t filaCheia =  PTHREAD_COND_INITIALIZER; 
pthread_cond_t filaVazia =  PTHREAD_COND_INITIALIZER; 


void produtor(void)
{
	int i = 0;
	while(i < 12)
	{
		
		if(n == MAXFILA)
		//se a fila estiver cheia,
		{
			//espera algum ser consumido
			pthread_cond_wait(&filaCheia, &mutex1);
		}
		
		
		
		//região crítica: altera o buffer
		pthread_mutex_lock(&lock);
		
		//buffer[n] = rand() % MAXRAND;
		buffer[n] = i;
		printf("%d produzido\n", buffer[n]);
		n++;
		pthread_cond_signal (&filaVazia);
		
		pthread_mutex_unlock(&lock); 
		i++;
		sleep(1);
	}
	printf("fim da produção\n");
	return;
}

void consumidor(void* id)
{
	int i = 0, t;
	printf("consumidor %d começou\n", (int) id);
	while(i < 4)
	{
		if(n == 0)
		//se o buffer está vazio,
		{
			//espera produção
			pthread_cond_wait(&filaVazia, &mutex2);
		}
		
		pthread_mutex_lock(&lock);
		
		printf("%d[%d] consumido.\n", buffer[0], (int)id);
		for(t = 0; t < n -1; t++)
		{
			buffer[t] = buffer[t+1];
		}
		n--;
		pthread_cond_signal (&filaCheia);
		
		pthread_mutex_unlock(&lock); 
		
		i++;
		
		sleep(2);
	}
	printf("fim do consumo\n");
	return;
}

int main(void)
{
	int i;
	pthread_t threads[NP + NC];
	for(i = 0; i < NP; i++)
	{
		pthread_create(&threads[i], NULL, produtor, NULL);
	}
	for(; i < NC + NP; i++)
	{
		pthread_create(&threads[i], NULL, consumidor, (void*) i);	
	}
	for(i = 0; i < NP + NC; i++)
	{
		pthread_join(threads[i],NULL);	
	}
	
	printf("final\n");
	return 0;    
}
