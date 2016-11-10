#include <sys/shm.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/sem.h>
#include <stdio.h>

union semun { 
	int val; 
	struct semid_ds *buf; 
	ushort *array; 
};

int setSemValue(int semId) { 
	union semun semUnion; 
	semUnion.val = 1; 
	return semctl(semId, 0, SETVAL, semUnion); 
}

void delSemValue(int semId) { 
	union semun semUnion; 
	semctl(semId, 0, IPC_RMID, semUnion); 
}

//perguntar sobre esses troço loko aqui em baixo

int semaforoP(int semId) { 
	struct sembuf semB; 
	semB.sem_num = 0; 
	semB.sem_op = -1; 
	semB.sem_flg = SEM_UNDO; 
	semop(semId, &semB, 1); 
	return 0; 
}

int semaforoV(int semId) { 
	struct sembuf semB; 
	semB.sem_num = 0; 
	semB.sem_op = 1; 
	semB.sem_flg = SEM_UNDO; 
	semop(semId, &semB, 1); 
	return 0; 
}

int main(void)
{
	//id do segmento de memória
	int segmento;
	//controle do semáforo
	int semId;
	//a variaível
	int *p;
	//segmento de memória compartilhada. NÃO CRIA
	segmento  = shmget (9000, sizeof(int), S_IRUSR | S_IWUSR);	
	// associa segmento de memória ao buffer
	p = (int*) shmat(segmento, 0,0);
	
	
	//o semáforo. NÂO CRIA
	semId = semget (8700, 1, 0666); 
	
	
	
	while(1)
	{
		semaforoP(semId);
		sleep(2);
		*p += 5;
		printf(" %d \n", *p);
		semaforoV(semId);
	}
	
	return 0;
}
	
	
