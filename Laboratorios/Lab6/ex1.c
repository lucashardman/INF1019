/* Exemplo de uso de semáforo*/ 

#include <sys/sem.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h> 

union semun { 
	int val; 
	struct semid_ds *buf; 
	ushort *array; 
};

// inicializa o valor do semáforo
int setSemValue(int semId);

// remove o semáforo
void delSemValue(int semId);

// operação P
int semaforoP(int semId);

//operação V
int semaforoV(int semId);

int main (int argc, char * argv[]) { 

	int i; 
	char letra = 'o'; 
	int semId; 

	if (argc > 1) { 
		semId = semget (8752, 1, 0666 | IPC_CREAT); 
		setSemValue(semId); 
		letra = 'x'; 
		sleep (2); 
	} 
	else{ 
		while ((semId = semget (8752, 1, 0666)) < 0) { 
			putchar ('.'); fflush(stdout); 
			sleep (1); 
		} 
	} 
	for (i=0; i<10; i++) { 
		semaforoP(semId); 
		putchar (toupper(letra)); fflush(stdout); 
		sleep (rand() %3); 
		putchar (letra); fflush(stdout); 
		semaforoV(semId); 
		sleep (rand() %2); 
	} 
	printf ("\nProcesso %d terminou\n", getpid()); 
	if (argc > 1) { 
		sleep(10); 
		delSemValue(semId); 
	} 
	return 0; 
}

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


/*
Explicação:

	O programa deve ser executado simultâneamente 1x em background(bg) e 1x em foreground(fg). O fg deve receber 1 argumento; assim, sua letra é x, enquanto no bg a letra é O. O processo em fg cria o semáforo, e enquanto isso o bg espera por ele checando se (semget (8752, 1, 0666)) < 0). A partir daí, ambos processos entram em um loop em que eles ligam o semáforo(semaforoP(semId)), imprimem suas letras(maiúscula ao entrar, minúscula ao sair) e desligam o semáforo(semaforoV(semId)). Como em semaforoP() usamos semop(), um processo espera o outro chamar semaforoV para prosseguir sua execução, e obtemos uma alternância em que vemos cada processo entrar e sair da "região crítica"(stdout).  Esse revezamento é feito através das funcoes semaforoP e semaforoV. A semaoforoV seta sem_op = 1, colocando-o em execução,e o semaforoP seta sem_op = 0, colocando-o em espera.

*/

/*
Explicação:

	Este programa recebe 1 como argumento para criar o semaforo utilizando a funcao semget com IPC_CREAT. Primeiramente
	devemos executa-lo em background, assim como o argumento é maior que 1 o processo entra em um loop que fica checando
	se o semaforo for criado (sai do loop quando semget for maior que 0).
	
	Quando executamos o programa em foreground devemos passar 1 como parametro. Dessa forma o processo que esta em background
	sai do loop e os dois processos entram num loop de 10 voltas, e passam a revezar a regiao critica (o output nesse caso).
	O processo que começou em background imprime 'O' quando entra na regiao critica e 'o' quando sai, e o processo que começou
	em foreground imprime 'X' quando entra na regiao critica em 'x' quando sai.
	
	Esse revezamento é feito através das funcoes semaforoP e semaforoV. A semaoforoV seta sem_op = 1, colocando-o em execução,
	e o semaforoP seta sem_op = 0, colocando-o em espera.
*/





