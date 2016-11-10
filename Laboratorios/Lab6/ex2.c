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
	//pids
	int pid;
	//controle do semáforo
	int semId;
	//contador
	int i;	
	//o buffer
	char *buffer;
	//cria segmento de memória compartilhada
	segmento  = shmget (IPC_PRIVATE, 16*sizeof(char), IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
	// associa segmento de memória ao buffer
	buffer = (char*) shmat(segmento, 0,0);
	
	buffer[0] = '0';
	buffer[15] = '0';
	
	semId = semget (8752, 1, 0666 | IPC_CREAT); 
	setSemValue(semId); 
	
	
	
	pid  = fork();
	if(pid == 0)
	//filho consumidor
	{
		sleep(2);
		while(1)
		{
			semaforoP(semId);
		
			i = 0;
			while(i < 16)
			{
				putchar(buffer[i]);
				i++;
			}
			fflush(stdout);
				
			semaforoV(semId);
			sleep(2);
		}
	}
	else
	//pai produtor
	{
		while(2)
		{
			semaforoP(semId);
		
			i = 0;
			while(i < 16)
			{
				scanf(" %c ", &buffer[i]);
				i++;
			}
				
			semaforoV(semId);
			sleep(2);
		}
	}
	
	// libera a memória compartilhada do processo
	shmdt (buffer); 
	// libera a memória compartilhada
	shmctl (segmento, IPC_RMID, 0); 
	delSemValue(semId); 
	return 0;
}
