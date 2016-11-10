#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

/*
Explicação:

O programa altera a entrada e a saida padrão e redirecionamos
para arquivos read.txt e temp.txt utilizando a função dup2().

Desta forma ao utilizar o scanf, o le de um arquivo e passa a informação
para a variavel recebido e a função printf le da variavel recebido
e imprime no temp.txt

*/

int main(void)
{
	char recebido[20];
	int fd, fr;
	
	fd = open("temp.txt",O_CREAT|O_RDWR|O_TRUNC,0666);
	fr = open("read.txt", O_RDWR,0666);
	
	dup2(fd, 1);
	
	dup2(fr, 0);
	
	
	
	scanf("%s", recebido);
	
	printf("%s", recebido);	
	
	return 0;
	
}
