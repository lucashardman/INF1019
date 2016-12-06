#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>

int split_buff (char string[], char ***ret);

void free_buf(int n, char ***buf);

int main (void){
	int n, i;
	char **buf = NULL;
	printf("%ld\n", &buf);
	n = split_buff("comando1, comando2, comando3", &buf);
	
	for(i = 0; i < n; i++)
	{
		printf("[%d]: %s\n", i, buf[i]);
	}
	free_buf(n, &buf);
	return 0;

}

int split_buff (char string[], char ***buf){

	char str[500]; // 500 = tamanho maximo do comando. Mudar para um valor mais correto.
	char *p, **ret ;
	int n_spaces = 0, i;
	ret = (char**) malloc(0 * sizeof(char*));

	strcpy(str, string);
	p =  strtok (str, ", ");

	//Divisao da string np vetor
	while (p != NULL) {
		ret = realloc (ret, sizeof (char*) * (n_spaces+1));

		if (ret == NULL){
			printf("Erro. Falha na alocacao de memoria.\n");
	    	exit (-1); 
		}
		
		ret[n_spaces] = (char*) malloc((strlen(p)+1) * sizeof(char));
		strcpy(ret[n_spaces], p);
		
		n_spaces++;
		
		p = strtok (NULL, ", ");
	}
	for(i = 0; i < n_spaces; i++)
	{
		printf("[%d]: %s\n", i, ret[i]);
	}
	printf("return %d\n", n_spaces);
	*buf = ret;
	return n_spaces;

}

void free_buf(int n,  char ***buf)
{
	int i;
	char **res = *buf;
	for(i = 0; i < n; i++)
	{
		free(res[i]);
	}
	free(**buf);
	return;
}






