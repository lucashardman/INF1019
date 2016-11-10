#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

//o programa cria um processo pai e filho. O filho muda a saída padrão para a escrita no pipe, e chama ls. A saída de ls é então escrita no pipe. O processo pai então muda a entrada padrão para a leitura do pipe. Quando wc é então chamado ele consome o que está no pipe, ou seja, a saída de ls, e usa.

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
	//processo filho
	{
		close(fd[0]);
		// muda a saída padrão para a escrita no pipe	
		if(dup2(fd[1], 1))
		{
			printf("Erro de dup2");
		}
		system("ls");
		close(fd[1]);
		return 0;
	}
	else
	//processo pai
	{
		close(fd[1]);
		//muda a entrada padrão para a escrita no pipe
		if(dup2(fd[0], 0))
		{
			printf("Erro de dup2");
		}
		system("wc");
		close(fd[0]);
		return 0;
	}
	
}
