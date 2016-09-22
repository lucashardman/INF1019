#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

void handler (int param);

int main (void) {

	int x,y;

	signal(SIGFPE, handler);

	printf("Digite o 1 numero: ");
	scanf("%d", &x);
	
	printf("Digite o 2 numero: ");
	scanf("%d", &y);

	printf("Adição: %d + %d = %d \n Subtração: %d - %d = %d \n Divisão: %d/%d = %d \n Multiplicação: %d x %d = %d \n", x, y, x+y, x, y, x-y, x, y, x/y, x, y, x*y);

	return 0;

}

void handler (int param) {

	printf ("Erro de floating point. Signal : %d\n", param);

	exit(0);

}

/*

Explicação:

Quando executamos o programa sem digitar o zero no segundo número, o programa não tenta uma divisão por zero.
Assim não há erro de floating number, então o sinal não é enviado e a função handler não é utilizada.

Já quando colocamos o segundo número como zero, na parte da divisão entre dois números, o númerador sendo zero cai no erro de
floating point. O programa automaticamente envia o sinal, gerando um código de erro default. Ao registrar o handler para este
sinal com "signal(SIGFPE, handler);", o tratamento de erro muda e a função handler é chamada.

Exemplo 1: sem zero no divisor.

[paranapanema.grad.inf.puc-rio.br:~/Documents/inf1019/Lab4] ./ex5
Digite o 1 numero: 1
Digite o 2 numero: 5
Adição: 1 + 5 = 6 
 Subtração: 1 - 5 = -4 
 Divisão: 1/5 = 0 
 Multiplicação: 1 x 5 = 5 

Exemplo 2: sem o registro do handler.

[paranapanema.grad.inf.puc-rio.br:~/Documents/inf1019/Lab4] ./ex5
Digite o 1 numero: 1
Digite o 2 numero: 0
Floating exception (core dumped)

Exemplo 3: com o registro do handler.

[paranapanema.grad.inf.puc-rio.br:~/Documents/inf1019/Lab4] ./ex5
Digite o 1 numero: 1
Digite o 2 numero: 0
Erro de floating point. Signal : 8

*/
