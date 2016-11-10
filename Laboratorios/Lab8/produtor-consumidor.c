#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define MAXFILA 8
#define MAXRAND 10

//buffer e contador de elementos no buffer
static int buffer[MAXFILA];
static int n = 0;
//mutex e variável de condição
static pthread_mutex_t lock;
static pthread_mutex_t mutex; 

pthread_cond_t fila =  PTHREAD_COND_INITIALIZER; 


void produtor(void)
{
	int i;
	for(i = 0; i < 64; i++)
	{
		
		if(n == MAXFILA)
		//se a fila estiver cheia,
		{
			//espera algum ser consumido
			pthread_cond_wait(&fila, &mutex);
		}
		
		
		
		//região crítica: altera o buffer
		pthread_mutex_lock(&lock);
		
		buffer[n] = rand() % MAXRAND;
		//printf("%d produzido\n", buffer[n]);
		n++;
		pthread_cond_signal (&fila);
		
		pthread_mutex_unlock(&lock); 
		sleep(1);
	}
	return;
}

void consumidor(void)
{
	int i, t;
	for(i = 0; i < 64; i++)
	{
		if(n == 0)
		//se o buffer está vazio,
		{
			//espera produção
			pthread_cond_wait(&fila, &mutex);
		}
		
		pthread_mutex_lock(&lock);
		
		printf("%d[%d] consumido.\n", buffer[0], i);
		for(t = 0; t < n -1; t++)
		{
			buffer[t] = buffer[t+1];
		}
		n--;
		pthread_cond_signal (&fila);
		
		pthread_mutex_unlock(&lock); 
		
		sleep(2);
	}
	return;
}

int main(void)
{
	pthread_t threads[2];
	pthread_create(&threads[0], NULL, produtor, NULL);
	pthread_create(&threads[1], NULL, consumidor, NULL);
	
	pthread_join(threads[0],NULL);
	pthread_join(threads[1],NULL);
	printf("final\n");
	return 0;    
}
