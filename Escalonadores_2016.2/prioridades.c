#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>

//tamanho máximo do nome. importante ser o mesmo definido em main.c
#define TAM 10

//Quantidade maxima de programas sendo escalonados
#define MAX_PROG 10

int fpFIFO;

//estrutura que armazena informações sobre um programa
typedef struct programa
{
	//nome do programa
	char nome[TAM];
	//prioridade do programa
	int prioridade;
	//pid do programa
	int pid;
	//true se o programa estiver terminado
	bool terminado;
} Programa;

Programa *lista;
int qtd=0;
int terminado=0;
bool reinicia=false;

void carrega_programa(int signo);
void encerra(int signo);
static int *ordemPrioridade(int quantidadeProgramas, int *ordem);

int main (int argc, char *argv[]) { 
	int pid=0, pidPai=0, status;
	int loop=0, i=0, j=0, contPrioridade=0;
	int ordem[MAX_PROG];

	time_t s; // o segundo corrente em cada loop
	time_t inicio; //o segundo em que o escalonador começa sua execução

	signal(SIGUSR1, carrega_programa);
	signal(SIGUSR2, encerra);

	//se fifo existe, abre fifo para leitura
	if(access("fifo", F_OK) == 0)
	{
		if((fpFIFO = open("fifo", O_RDONLY)) <= 0)
		{
			printf("erro ao abrir fifo\n");
			return -2;
		}
	}
	//FIM: se fifo existe, abre fifo para leitura

	

	for(i=0;i<=7;i++){ //Loop para cada prioridade

		j=0;
		contPrioridade = 0;

		sleep(3); //Espera 3 segundos para dar tempo de carregar alguns programas.
		//OBS: Se os programas com prioridades maiores forem carregador depois 
				
		fflush(stdout);	
		
		for(j=0;j<qtd;j++){
			if(lista[j].prioridade == i){
				contPrioridade++;
			}
		}
		if(contPrioridade == 0){
			if(i==7){
				i=0;
			}
			continue;
		}
		else{
			printf("Escalonando %d programas de prioridade %d\n", contPrioridade, i);
		}

		j=0;
		while(contPrioridade > 0){ //Loop importante para ficar esperando novos programas
			
			for(loop=0;loop<qtd;loop++){ // Este loop eh apenas para garantir que os programas estao todos em estado de espera antes do escalonamento comecar.
				kill(lista[loop].pid, SIGSTOP);
			}

			fflush(stdout);	

			if(lista[j].prioridade == i && lista[j].terminado == false){
					
				printf("%s de prioridade %d vai recomecar.\n", lista[j].nome, i);

				if(waitpid(lista[j].pid, &status, WNOHANG) != 0){ //Waitpid != 0, se o processo com pid passado como argumento estiver terminado
					printf("--> Programa %s terminado\n", lista[j].nome);
					lista[j].terminado = true;
					lista[j].prioridade = 0; //Isto faz com que a prioridade deste programa n~ao se repita.
					contPrioridade--;
				}
				for(loop=0;loop<qtd;loop++){//Testa se todos os programas terminaram.
					if(lista[loop].terminado == true){
						terminado++;
					}
				}
				if(terminado == qtd){//Se todos programas terminaram, termina.
					printf("--> Todos os programas terminaram.\n");
					kill(pidPai, SIGUSR2);
					return 0;
				}
				terminado = 0;

				fflush(stdout);
				sleep(1);
				kill(lista[j].pid, SIGCONT); // O programa eh escalonado. Entra em estado de execucao.
				fflush(stdout);
				sleep(3); // O programa executa por 3 segundos.
				kill(lista[j].pid, SIGSTOP); // O programa passa a vez. Entra em estado de espera.
				
				if(reinicia == true){	// Break para quando acabar a fatia de tempo, o programa de prioridade maior que entrou comecar.
					i=0;
					break;
				}
			}
			j++;
			if(j==qtd){
				j=0;
			}
		}

		//O flag reinicia serve pra recomecar a lista de prioridades caso um programa com prioridade mais alta
		//seja carregado depois que um programa de priridade mais baixa ja comecou.
		if(reinicia == true){
			
			i=0;
			reinicia=false;
			continue;
		}

		//Serve para deixar o loop infinito, pois sem isso nao eh possivel ESPERAR carregar todos os programas
		if(i==7){
			i=0;
		}
	}
} 

/****************************************************************************
 * Carrega_programa                                                         *
 * 1 - Esta funcao eh um handler para o sinal SIGUSR1.                      *
 * 2 - Le a FIFO e utiliza os dado lidos para inicializar um programa e     *
 * coloca-lo em estado de espera.                                           *
 * 3 - Os pid sao armazenados   *
 * 4 - Estrutura recebe os dados                                            *
 ****************************************************************************/
void carrega_programa(int signo){

	int prioridade;
	char nome_aux[TAM];
	int pid, status;
	int pid_teste;
	read(fpFIFO, nome_aux, TAM);
	read(fpFIFO, &prioridade, sizeof(int));

	//sabendo que o intervalo desejado é compatível, adiciona ele na lista e no vetor programacao
	//lista
	lista = realloc(lista, (qtd+1)*sizeof(Programa));
	if(lista == NULL){
		printf("Erro de alocação de memória\n");
		exit(1);
	}
	lista[qtd].prioridade = prioridade;

	pid = fork();

	strcpy(lista[qtd].nome,"");
	strcat(lista[qtd].nome, "./");
	strcat(lista[qtd].nome, nome_aux);

	
	
	if(pid!=0){
		lista[qtd].pid = pid;
	}
	else{
		lista[qtd].terminado = false;
		execve(lista[qtd].nome, NULL, NULL);
	}
	
	kill(lista[qtd].pid, SIGSTOP);

	printf("Programa %s de PID %d e prioridade %d carregado.\n", lista[qtd].nome, lista[qtd].pid, lista[qtd].prioridade);
	fflush(stdout);
	qtd++;
	reinicia = true;
}

//função chamada para fechar a fifo
void encerra(int signo){	
	close(fpFIFO); // O vetor ordem armazena a ordem de programas que estao sendo escalonados. 
	return;
}