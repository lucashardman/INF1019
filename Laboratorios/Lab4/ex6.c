#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

int flag = 0;
int time = 0;
int value = 0;

void initHandler (int sinal);
void quitHandler (int sinal);

int main (void){

	signal(SIGUSR1, initHandler);
	signal(SIGUSR2, quitHandler);

	printf("Preparado\n");
	
	pause();
	
	return 0;
}

void initHandler (int sinal){

	printf("Ligação iniciada.\n");
	while (1){

		if(flag == 0){

			sleep(1);

			time++;

			if(time <= 60)
				value = value + 2;
			else
				value++;
		}
	}
}

void quitHandler (int sinal){

	flag = 1;
	printf("Terminado.\nTempo gasto: %d segundos\nPreco: %d centavos\n", time, value);
	pause();

	exit(0);
}

/*

Explicação:

O programa é iniciado em backgroud e fica esperando o inicio de uma ligação.

A ligação é feita através do sinal initHandler, que é chamado pelo sinal SIGUSR1.
O usuário manda o sinal através do comando "kill -s USR1 3004" (levando em conta que 3004 é o pid do processo),
e assim a ligação é iniciada.

O mesmo é feito para terminar uma ligação através do quitHandler, que é chamado pelo sinal SIGUSR2.
Este sinal é chamado no terminal pelo comando "kill -s USR1 3004" (levando em conta que 3004 é o pid do processo).
A função quitHandler diz a duração da chamada, o preço da mesma e termina o programa.

Exempĺo 1: menos de 1 minuto.

[paranapanema.grad.inf.puc-rio.br:~/Documents/inf1019/Lab4] ./ex6 &
[14] 3315
[paranapanema.grad.inf.puc-rio.br:~/Documents/inf1019/Lab4] Preparado

[paranapanema.grad.inf.puc-rio.br:~/Documents/inf1019/Lab4] kill -s USR1 3315
Ligação iniciada.
[paranapanema.grad.inf.puc-rio.br:~/Documents/inf1019/Lab4] kill -s USR2 3315
Terminado.
Tempo gasto: 6 segundos
Preco: 12 centavos


Exemplo 2: mais de 1 minuto.

[paranapanema.grad.inf.puc-rio.br:~/Documents/inf1019/Lab4] ./ex6 &
[14] 3374
[paranapanema.grad.inf.puc-rio.br:~/Documents/inf1019/Lab4] Preparado

[paranapanema.grad.inf.puc-rio.br:~/Documents/inf1019/Lab4] kill -s USR1 3374
Ligação iniciada.
[paranapanema.grad.inf.puc-rio.br:~/Documents/inf1019/Lab4] kill -s USR2 3374
Terminado.
Tempo gasto: 84 segundos
Preco: 144 centavos

*/
