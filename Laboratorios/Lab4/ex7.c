#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

int main (void) {

	int pid1, pid2, pid3;

	pid1 = fork();
	if(pid1 == 0){
		execve("./prog1", NULL, NULL);
	}
	kill(pid1, SIGSTOP);
	
	pid2 = fork();
	if(pid2 == 0){
		execve("./prog2", NULL, NULL);
	}
	kill(pid2, SIGSTOP);
	
	pid3 = fork();
	if(pid3 == 0){
		execve("./prog3", NULL, NULL);
	}
	kill(pid3, SIGSTOP);
	
	while(1){

		kill(pid3, SIGSTOP);	
		kill(pid1, SIGCONT);
		sleep(1);
		
		kill(pid1, SIGSTOP);
		kill(pid2, SIGCONT);
		sleep(2);
		
		kill(pid2, SIGSTOP);
		kill(pid3, SIGCONT);
		sleep(2);
	}
	
	return 0;

}

/*

Explicação:

Primeiramente são criados 3 processos, cada um deles inicia um programa diferente com o 
comando execve e depois põe o processo em estado de espera.

Depois temos um loop sem fim que faz a alternancia entre os 3 processos. Manda um sinal para
colocar o processo que estava executando anteriormente em espera e colocar o próximo em
execução e depois esperar até a próxima alternancia com sleep.

*/
