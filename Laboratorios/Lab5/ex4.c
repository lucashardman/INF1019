#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

// a função cria um pai com dois filhos. O pai escreve no pipe em intervalos de tempo,  inteiros consecutivos(i), e os filhos leem os valores do pipe e os imprimem. Vemos que os filhos imprimem na tela alternadamente os valores, já que ambos consomem assim que podem o que há no pipe.

int main(void)
{
	int fd[2];
	int pid;
	
	if(pipe(fd) < 0)
	{
		printf("Erro ao criar Pipe.\n");
		exit(1);
	}
	if((pid = fork()) == 0)
	//processo leitor filho1
	{
		close(fd[1]);
		int i;
		while(1)
		{
			read(fd[0], &i, sizeof(int));
			printf("%d é do filho 1!\n", i);
			sleep(2);
		}
	}
	else
	//processo pai
	{
		if((pid = fork()) == 0)
		//processo filho2
		{
			close(fd[1]);
			int i;
			while(1)
			{
				read(fd[0], &i, sizeof(int));
				printf("%d é do filho 2!\n", i);
				sleep(2);
			}
		}
		else
		//pai escritor
		{
			close(fd[0]);
			int i = 0;
			while(1)
			{
				write(fd[1], &i, sizeof(int));
				i++;
				sleep(1);
			}
		}
	}
	
}
