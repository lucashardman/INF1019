#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

#include "escalonadores.h"

/*************************** Variaveis globais estaticas **********************/
static int metodoEscalonamento;
/***************************************
 * metodoEscalonamento:                *
 * esta variavel serve para as funcoes *
 * estaticas secundarias identificarem *
 * qual eh a politica de escalonamento *
 * que esta ocorrendo.                 *
 * 1 - Por prioridade                  *
 * 2 - Round-Robin                     *
 * 3 - Por loteria                     *
 ***************************************/

static ProgramaPrioridade *progPrioridade[maximo_programas]; //Estrutura responsavel pelo controle dos programas pela politica por prioridade
static ProgramaRoundRobin *progRoundRobin[maximo_programas]; //Estrutura responsavel pelo controle dos programas pela politica por round robin
static ProgramaLoteria *progLoteria[maximo_programas]; //Estrutura responsavel pelo controle dos programas pela politica por loteria

/****************************** Funcoes estaticas ***************************/
static void iniciarProgramas(int quantidadeProgramas, int *pid);
static bool testaProgramasFinalizados(int quantidadeProgramas, int *quantidadeRodando);
static bool contidoNoVetor(int valor, int *vetor, int tamanho);
static void distribuicaoBilhetes(int quantidadeProgramas, int quantidadeBilhetes);
static int sorteioBilhete(int limite);
static char *menorPrioridade(int quantidadeProgramas);

/************************ Funcoes de escalonamento **************************/

/****************************************************************************
 * Politica de escalonamento por prioridade                                 *
 * Descricao: nesta politica de escalonamento, cada programa tem uma        *
 * prioridade. Quanto menor o valor da prioridade, maior eh a prioridade,   *
 * ou seja, prioridade 1 >> prioridade 7.                                   *
 * Parametros:                                                              *
 * quantidadeProgramas - quantidade de programas sendo gerenciados          *
 * *programas - lista de programas sendo gerenciados                        *
 ****************************************************************************/
void escalonamentoPorPrioridade(int quantidadeProgramas, ProgramaPrioridade *programas[maximo_programas]){

	int loop1 = 0; //Variaveis auxiliares para loop
	int pid[maximo_programas]; //Variaveis responsaveis por guardar os pid dos processos

	int quantidadeRodando = quantidadeProgramas; //Quantidade de processos em execucao
	float contadorTempo = 0; //Responsavel por medir o tempo que cada programa demora pra executar

	int waitpidResult = 0; //Variavel para guardar os resultados dos retornos da funcao waitpid
	int waitpidStatus = 0; //Variavel para guardar o estado do processo pelo waitpid

	metodoEscalonamento = 1; //Por Prioridade

	/* Inicializa progPrioridade e executa os programas e envia o sinal para entrarem em espera */
	for(loop1=0;loop1<quantidadeProgramas;loop1++){
		progPrioridade[loop1] = programas[loop1];
	}
	iniciarProgramas(quantidadeProgramas, pid);
	/* Fim: Inicializa progPrioridade e executa os programas e envia o sinal para entrarem em espera */

	/* Inicializar o campo "terminado" com false */
	for(loop1=0;loop1<quantidadeProgramas;loop1++){
		progPrioridade[loop1]->terminado = false;
	}
	/* Fim: Inicializar o campo "terminado" com false */

	/* Inicializar variavel de contagem de tempo de execucao */
	for(loop1=0;loop1<quantidadeProgramas;loop1++){
		progPrioridade[loop1]->tempoExecucao = 0;
	}
	/* Fim: Inicializar variavel de contagem de tempo de execucao */

	for(loop1=0;loop1<quantidadeProgramas;loop1++){
		printf("O programa: %s de pid %d foi iniciado.\n", progPrioridade[loop1]->nome, pid[loop1]);
	}

	printf("\n");

	/* Inicio do algoritmo de Prioriodade */
	printf("Menor prioridade: %s\n",menorPrioridade(quantidadeProgramas));
	/* Fim do algoritmo de Prioriodade */
}

/****************************************************************************
 * Politica de escalonamento Round-Robin                                    *
 * Descricao: cada programa possui a mesma fatia de tempo pra executar. De  *
 * acordo com a ordem de chamada de cada programa eles executam durante o   *
 * seu tempo de forma circular.                                             *
 * Parametros:                                                              *
 * quantidadeProgramas - quantidade de programas sendo gerenciados          *
 * *programas - lista de programas sendo gerenciados                        *
 ****************************************************************************/
void escalonamentoRoundRobin(int quantidadeProgramas, ProgramaRoundRobin *programas[maximo_programas]){

	int loop1 = 0; //Variaveis auxiliares para loop
	int pid[maximo_programas]; //Variaveis responsaveis por guardar os pid dos processos
	int timeSharing = 500000; //Tempo reservado para a execucao de cada programa em microssegundos

	int quantidadeRodando = quantidadeProgramas; //Quantidade de processos em execucao
	float contadorTempo = 0; //Responsavel por medir o tempo que cada programa demora pra executar

	int waitpidResult = 0; //Variavel para guardar os resultados dos retornos da funcao waitpid
	int waitpidStatus = 0; //Variavel para guardar o estado do processo pelo waitpid

	metodoEscalonamento = 2; //Round-Robin

	/* Inicializa progRoundRobin e executa os programas e envia o sinal para entrarem em espera */
	for(loop1=0;loop1<quantidadeProgramas;loop1++){
		progRoundRobin[loop1] = programas[loop1];
	}
	iniciarProgramas(quantidadeProgramas, pid);
	/* Fim: Inicializa progRoundRobin e executa os programas e envia o sinal para entrarem em espera */

	/* Inicializar o campo "terminado" com false */
	for(loop1=0;loop1<quantidadeProgramas;loop1++){
		progRoundRobin[loop1]->terminado = false;
	}
	/* Fim: Inicializar o campo "terminado" com false */

	/* Inicializar variavel de contagem de tempo de execucao */
	for(loop1=0;loop1<quantidadeProgramas;loop1++){
		progRoundRobin[loop1]->tempoExecucao = 0;
	}
	/* Fim: Inicializar variavel de contagem de tempo de execucao */

	printf("Fatia de tempo: %.2f segundos\n\n", (float) timeSharing/1000000);

	for(loop1=0;loop1<quantidadeProgramas;loop1++){
		printf("O programa: %s de pid %d foi iniciado.\n", progRoundRobin[loop1]->nome, pid[loop1]);
	}

	printf("\n");

	/* Inicio do algoritmo de Round-Robin */
	loop1 = 0;
	while(1){

		if(testaProgramasFinalizados(quantidadeProgramas, &quantidadeRodando) == true){ // Testa se todos os programas ja foram finalizados

			printf("\nFim da execucao de todos programas pela politica de escalonamento Round-Robin\nTempo total de execucao dos progaras: %.2f\n", contadorTempo/1000);
			return; //Finaliza a funcao de Round-Robin
		}
		
		if(progRoundRobin[loop1]->terminado == false){

			kill(pid[loop1], SIGCONT); //Sinal para o programa entrar em estado de execucao
			fflush(stdout);
			
			usleep(timeSharing); //0.5 segundos = 500.000 microssegundos

			/************************************************************
			 * - waitpidResult < -1: espera por qualquer processo filho *
			 * que seu grupo ID eh igual ao valor absoluto de pid.      *
			 * - waitpidResult = -1: espera por qualquer processo filho.*
			 * - waitpidResult = 0: espera por qualquer processo filho  *
			 * que o seu grupo ID seja igual ao do processo que chama.  *
			 * - waitpid < 0: espera pelo processo filho que o grupo ID *
			 * eh igual ao valor do pid.                                *
			 ************************************************************/
			waitpidResult = waitpid(pid[loop1], &waitpidStatus, WNOHANG);

			/* Conta quanto tempo esta se passando ao longo da execucao dos programas */
			contadorTempo = contadorTempo + timeSharing/1000;

			if(waitpidResult == 0){

				progRoundRobin[loop1]->tempoExecucao = contadorTempo;
				kill(pid[loop1], SIGSTOP); //Sinal para o programa entrar em estado de espera
				loop1++;
			}
			else{ //Fim da execucao de um programa
				progRoundRobin[loop1]->tempoExecucao = contadorTempo;
				printf("O programa %s terminou em %.2f segundos.\n", progRoundRobin[loop1]->nome, progRoundRobin[loop1]->tempoExecucao/1000);
				fflush(stdout);
				progRoundRobin[loop1]->terminado = true;
				loop1++;
			}
		}
		else{
			loop1++;
		}

		//Condicional responsavel por manter o loop1 rodando entre 0 e a quantidade de programas
		if(loop1 == quantidadeProgramas)
			loop1 = 0;

		fflush(stdout);

		
	}
	/* Fim do algoritmo de Round-Robin */
}

/****************************************************************************
 * Politica de escalonamento por Loteria                                    *
 * Descricao: os programas possuem a mesma fatia de tempo para executar.    *
 * Os programas possuem uma lista de bilhetes de 1 a 20, que sao sorteados  *
 * a cada loop. O programa que possui o bilhete sorteado sera o proximo a   *
 * ser executado.                                                           *
 * Parametros:                                                              *
 * quantidadeProgramas - quantidade de programas sendo gerenciados          *
 * *programas - lista de programas sendo gerenciados                        *
 ****************************************************************************/
void escalonamentoLoteria(int quantidadeProgramas, ProgramaLoteria *programas[maximo_programas]){

	int loop1 = 0, loop2 = 0, loop3 = 0; //Variaveis auxiliares para loop
	int pid[maximo_programas]; //Variaveis responsaveis por guardar os pid dos processos
	int timeSharing = 500000; //Tempo reservado para a execucao de cada programa em microssegundos

	float contadorTempo = 0;

	int waitpidResult = 0; //Variavel para guardar os resultados dos retornos da funcao waitpid
	int waitpidStatus = 0; //Variavel para guardar o estado do processo pelo waitpid

	int quantidadeBilhetes=0; //Guarda o numero de bilhetes total (soma da quantidade de bilhetes de cada programa)
	int quantidadeRodando = quantidadeProgramas; // Guarda a quantidade de programas em execucao

	metodoEscalonamento = 3; //Por Loteria

	/* Inicializacao de progLoteria */
	for(loop1=0;loop1<quantidadeProgramas;loop1++){
		progLoteria[loop1] = programas[loop1];
	}
	/* Fim: Inicializacao de progLoteria */

	/* Calcula quantidade total de bilhetes que devem ser gerados */
	quantidadeBilhetes = 0;
	for(loop1=0;loop1<quantidadeProgramas;loop1++){
		quantidadeBilhetes = quantidadeBilhetes + progLoteria[loop1]->quantidadeBilhetes;
	}
	if(quantidadeBilhetes>20){ //Se a quantidade de bilhetes lidos de exec.txt for superior a 20 o programa termina
		printf("Erro no arquivo exec.txt. A quantidade maxima de bilhetes eh 20.\nQuantidade: %d\n", quantidadeBilhetes);
		exit(1);
	}
	/* Fim: Calcula quantidade total de bilhetes que devem ser gerados */

	/* Sorteio de bilhetes */
	distribuicaoBilhetes(quantidadeProgramas, quantidadeBilhetes);
	/* Fim: Sorteio de bilhetes */

	iniciarProgramas(quantidadeProgramas, pid); //Executa os programas e envia o sinal para entrarem em espera

	/* Inicializar o campo "terminado" com false */
	for(loop1=0;loop1<quantidadeProgramas;loop1++){
		progLoteria[loop1]->terminado = false;
	}
	/* Fim: Inicializar o campo "terminado" com false */

	/* Inicializar variavel de contagem de tempo de execucao */
	for(loop1=0;loop1<quantidadeProgramas;loop1++){
		progLoteria[loop1]->tempoExecucao = 0;
	}
	/* Fim: Inicializar variavel de contagem de tempo de execucao */

	printf("Fatia de tempo: %.2f segundos\n\n", (float) timeSharing/1000000);

	for(loop1=0;loop1<quantidadeProgramas;loop1++){
		printf("O programa: %s de pid %d foi iniciado.\n", progLoteria[loop1]->nome, pid[loop1]);
	}
	printf("\n");

	/* Inicio do algoritmo de escalonamento por loteria */

	loop1 = 1; //Bilhetes vao de 1 a 20, e nao de 0 a 19
	while(1){

		loop1 = sorteioBilhete(quantidadeBilhetes); //Bilhetes vao de 1 a 20, e nao de 0 a 19

		printf("\n********** Bilhete %d **********\n", loop1);

		if(testaProgramasFinalizados(quantidadeProgramas, &quantidadeRodando) == true){ // Testa se todos os programas ja foram finalizados

			printf("\nFim da execucao de todos programas pela politica de escalonamento por Loteria\nTempo total de execucao dos progaras: %.2f\n", contadorTempo/1000);
			return; //Finaliza a funcao de Round-Robin
		}

		for(loop2=0;loop2<quantidadeProgramas;loop2++){ //Verificar para cada bilhete se ele esta contido no programa
			for(loop3=0;loop3<progLoteria[loop2]->quantidadeBilhetes;loop3++){ //Comparar o bilhete da vez com cada bilhete do programa atual

				if(loop1 == progLoteria[loop2]->bilhetes[loop3]){

					kill(pid[loop2], SIGCONT); //Sinal para o programa entrar em estado de execucao
					fflush(stdout);
					
					usleep(timeSharing); //0.5 segundos = 500.000 microssegundos

					/************************************************************
					 * - waitpidResult < -1: espera por qualquer processo filho *
					 * que seu grupo ID eh igual ao valor absoluto de pid.      *
					 * - waitpidResult = -1: espera por qualquer processo filho.*
					 * - waitpidResult = 0: espera por qualquer processo filho  *
					 * que o seu grupo ID seja igual ao do processo que chama.  *
					 * - waitpid < 0: espera pelo processo filho que o grupo ID *
					 * eh igual ao valor do pid.                                *
					 ************************************************************/
					waitpidResult = waitpid(pid[loop2], &waitpidStatus, WNOHANG);

					/* Conta quanto tempo esta se passando ao longo da execucao dos programas */
					contadorTempo = contadorTempo + timeSharing/1000;

					if(waitpidResult == 0){

						progLoteria[loop2]->tempoExecucao = contadorTempo;
						kill(pid[loop2], SIGSTOP); //Sinal para o programa entrar em estado de espera
					}
					else{ //Fim da execucao de um programa
						if(progLoteria[loop2]->terminado != true){
							progLoteria[loop2]->tempoExecucao = contadorTempo;
							printf("O programa %s terminou em %.2f segundos.\n", progLoteria[loop2]->nome, progLoteria[loop2]->tempoExecucao/1000);
							fflush(stdout);
							progLoteria[loop2]->terminado = true;
						}
						else{
							printf("O programa %s ja finalizou e nao precisou do bilhete.\n", progLoteria[loop2]->nome);
						}
					}
				}
			}
			fflush(stdout);
		}

	}
	/* Fim do algoritmo de escalonamento por loteria */
}

/****************************** Funcoes estaticas ***************************/

/****************************************************************************
 * Nome: iniciarProgramas                                                   *
 * Descricao: responsavel por iniciar os programas que serao escalonados    *
 * utilizando execve e enviar um sinal para eles entrarem em espera SIGSTOP *
 * Parametros:                                                              *
 * quantidadeProgramas - quantidade de programas sendo gerenciados          *
 * pid - vetor responsavel por retornar o pid dos programas                 *
 ****************************************************************************/
static void iniciarProgramas(int quantidadeProgramas, int *pid){

	int loop = 0;
	int pid_auxiliar = 0; //Variaveis responsaveis por guardar os pid dos processos

	/* Executa os programas e envia o sinal para entrarem em espera */
	for(loop=0;loop<quantidadeProgramas;loop++){

		pid_auxiliar = fork();

		if(pid_auxiliar!=0){

			pid[loop] = pid_auxiliar;
			kill(pid_auxiliar, SIGSTOP);
		}
		else{
			sleep(3); //Programas iniciados com uma diferenca de 3 segundos entre cada
			if(metodoEscalonamento == 1){
				execve(progPrioridade[loop]->nome,NULL,NULL);
			}
			else if(metodoEscalonamento == 2){
				execve(progRoundRobin[loop]->nome,NULL,NULL);
			}
			else if(metodoEscalonamento == 3){
				execve(progLoteria[loop]->nome,NULL,NULL);
			}
			else{
				printf("Problema com a identificacao do metodo de escalonamento na funcao iniciarProgramas\nPor favor, reinicie o programa.\n");
				exit(1);
			}
		}
	}
	/* FIM: Executa os programas e envia o sinal para entrarem em espera */
}

/****************************************************************************
 * Nome: testaProgramasFinalizados                                          *
 * Descricao: responsavel por testar se todos os programas ja foram         *
 * finalizados.                                                             *
 * Parametros:                                                              *
 * quantidadeProgramas - quantidade de programas sendo gerenciados          *
 * quantidadeRodando - retorna por ponteiro um inteiro que representa       *
 * a quantidade programas ainda sendo executados.                           *
 * Condicoes de retorno:                                                    *
 * true - todos os programas ja foram finalizados                           *
 * false - ainda faltam programas a serem finalizados                       *
 ****************************************************************************/
static bool testaProgramasFinalizados(int quantidadeProgramas, int *quantidadeRodando){

	int loop = 0;

	/**********************************************************************
	//Bloco de teste:       
	// printf("Teste de alcance para funcao testaProgramasFinalizados\n");
	//
	// printf("Lista de programas:\n");
	// for(loop=0;loop<quantidadeProgramas;loop++)
	// 	printf("%s\n", progRoundRobin[loop]->nome);
	/**********************************************************************/

	*quantidadeRodando = quantidadeProgramas;

	for(loop=0;loop<quantidadeProgramas;loop++){
		
		if(metodoEscalonamento == 1){
			if(progPrioridade[loop]->terminado == true){
				*quantidadeRodando = *quantidadeRodando -1;
			}
		}
		if(metodoEscalonamento == 2){
			if(progRoundRobin[loop]->terminado == true){
				*quantidadeRodando = *quantidadeRodando -1;
			}
		}
		if(metodoEscalonamento == 3){
			if(progLoteria[loop]->terminado == true){
				*quantidadeRodando = *quantidadeRodando -1;
			}
		}
	}

	//printf("Quantidade de programas rodando: %d\n", *quantidadeRodando);


	if(*quantidadeRodando == 0){
		//printf("Todos terminados\n");
		return true; // Retorna aviso de que todos os programas foram finalizados
	}

	/* TESTE
	for(loop=0;loop<quantidadeProgramas;loop++)
		if(progRoundRobin[loop]->terminado == false)
			printf("Falta %s\n", progRoundRobin[loop]->nome);
	*/

	return false; //Retorna aviso de que ainda faltam programas a serem finalizados
}

/****************************************************************************
 * Nome: distribuicaoBilhetes                                               *
 * Descricao: responsavel pela geracao e distriguicao de bilhetes para os   *
 * programas que serao executados pelo escalonamento por loteria            *
 * Parametros:                                                              *
 * quantidadeProgramas - quantidade de programas sendo gerenciados          *
 ****************************************************************************/
static void distribuicaoBilhetes(int quantidadeProgramas, int quantidadeBilhetes){

	long bilhete;
	int limite = 20;
	int loop1 = 0, loop2 = 0, loop3 = 0;
	int *vetorBilhetes;
	bool contido=true;

	unsigned long num_bins = (unsigned long) limite +1;
	unsigned long num_rand = (unsigned long) RAND_MAX +1;
	unsigned long bin_size = num_rand / num_bins;
	unsigned long defect = num_rand % num_bins;

	/* Alocacao de memoria  para vetorBilhetes */
	vetorBilhetes = (int *) malloc (quantidadeBilhetes * sizeof(int));
	if(vetorBilhetes == NULL){
		printf("Erro na distribuicao de bilhetes. Memoria para vetorBilhetes insuficiente.\n");
		exit(1);
	}
	/* Fim: Alocacao de memoria  para vetorBilhetes */

	/********************************************************************************
	 * Gera vetor de numeros aleatorios e diferentes entre 0 e quantidadeBilhetes   *
	 * 1 - No algoritmo abaixo, um numero aleatorio eh gerado atraves do do-while.  *
	 * 2 - Um if verifica se o numero gerado ja esta dentro do vetor.               *
	 * 3 - Se o numero gerado ainda nao estiver no vetor, o vetor recebe o numero   *
	 * e um contador de loop eh incrementado.                                       *
	 * 4 - quando o loop chega ao numero de bilhetes que todos os programas possuem *
	 * somados, o loop para.                                                        *
	*********************************************************************************/
	loop1 = 0;
	while(loop1 != quantidadeBilhetes){

		do{
			bilhete = random();
		}while(num_rand - defect <=(unsigned long) bilhete);

		if(contidoNoVetor((int) bilhete/bin_size, vetorBilhetes, quantidadeBilhetes) == false){ 
			vetorBilhetes[loop1]=(int) bilhete/bin_size;
			loop1++;				
		}
	}
	/* Fim: Gera vetor de numeros aleatorios e diferentes entre 0 e quantidadeBilhetes */

	/* Distribui os bilhetes sorteados entre os programas */
	loop1=0;
	loop2 = 0;
	for(loop3=0;loop3<quantidadeBilhetes;loop3++){ //loop3 representa cada bilhete contido em vetorBilhetes
			
		if(loop2 == progLoteria[loop1]->quantidadeBilhetes){ //Se loop2 alcancou o ultimo espaco para os bilhetes de progLoteria[loop1]
	
			loop1++;//Passa para o proximo programa
			loop2 = 0; //Passa para o primeiro bilhete do programa q atual (q passou a ser depois de loop1++)
			progLoteria[loop1]->bilhetes[loop2] = vetorBilhetes[loop3];
			//TESTE printf("\n%d - %s recebe bilhete %d\n",vetorBilhetes[loop3],progLoteria[loop1]->nome, progLoteria[loop1]->bilhetes[loop2]);
			loop2++; //Passa para o proximo bilhete
		}
		else{ //Se loop2 ainda nao alcancou o ultimo espaco para os bilhetes de progLoteria[loop1]
			progLoteria[loop1]->bilhetes[loop2] = vetorBilhetes[loop3];
			//TESTE printf("\n%d - %s recebe bilhete %d\n",vetorBilhetes[loop3],progLoteria[loop1]->nome, progLoteria[loop1]->bilhetes[loop2]);
			loop2++; //Passa para o proximo bilhete
		}
	}
	/* Fim: Distribui os bilhetes sorteados entre os programas */

	/* Print dos bilhetes de cada programa */
	printf("\n");
	for(loop1=0;loop1<quantidadeProgramas;loop1++){ //Loop para cada programa

		printf("Bilhetes do %s: {", progLoteria[loop1]->nome);
		for(loop2=0;loop2<progLoteria[loop1]->quantidadeBilhetes;loop2++){ //Para cada bilhete de cada programa

			printf("%d", progLoteria[loop1]->bilhetes[loop2]);

			if(loop2 != progLoteria[loop1]->quantidadeBilhetes-1)
				printf(", ");
		}
		printf("}\n\n");
	}
	/* Fim: print dos bilhetes de cada programa */
}

/****************************************************************************
 * Nome: contidoNoVetor                                                     *
 * Descricao: responsavel pelo sorteio de bilhetes.                         *
 * Parametros:                                                              *
 * valor - valor que se deseja consultar se esta contido                    *
 * vetor - vetor que vai ser consultado se tem o valor                      *
 * tamanho - tamanho do vetor                                               * 
 * Condicoes de retorno:                                                    *
 * true - esta contido                                                      *
 * false - nao esta contido                                                 *
 ****************************************************************************/
static bool contidoNoVetor(int valor, int *vetor, int tamanho){

	int loop;
    for (loop=0; loop < tamanho; loop++) {
        if (vetor[loop] == valor)
            return true;
    }
    return false;
}

/****************************************************************************
 * Nome: sorteioBilhetes                                                    *
 * Descricao: gera um numero aleatorio de 1 ate limite.                     *
 * Parametros:                                                              *
 * limite - o maior valor que pode ser gerado                               *
 ****************************************************************************/
static int sorteioBilhete(int limite){

	long bilhete;
	int sorteado = 0;

	unsigned long num_bins = (unsigned long) limite +1;
	unsigned long num_rand = (unsigned long) RAND_MAX +1;
	unsigned long bin_size = num_rand / num_bins;
	unsigned long defect = num_rand % num_bins;

	/********************************************************************************
	 * Gera vetor de numeros aleatorios e diferentes entre 0 e quantidadeBilhetes   *
	 * 1 - No algoritmo abaixo, um numero aleatorio eh gerado atraves do do-while.  *
  	 * 2 - O numero gera eh o retorno da funcao                                     *
	*********************************************************************************/

	while (sorteado == 0){
		do{
			bilhete = random();
		}while(num_rand - defect <=(unsigned long) bilhete);

		sorteado = (int) bilhete/bin_size;
		/* Fim: Gera vetor de numeros aleatorios e diferentes entre 0 e quantidadeBilhetes */
	}
	return sorteado;
}

/****************************************************************************
 * Nome: menorPrioridade                                                    *
 * Descricao: serve para informar o programa rodando com menor prioridade   *
 * Parametros:                                                              *
 * limite - o maior valor que pode ser gerado                               *
 * Retorno:                                                                 *
 * nomeMenor - retorna uma string que eh o nome do programa com menor       *
 * prioridade.                                                              *
 ****************************************************************************/
static char *menorPrioridade(int quantidadeProgramas){

	int loop = 0, loop2 = 0;
	int menor = 0;

	int quantidadeRodando;
	int *rodando;

	char *nomeMenor;

    ProgramaPrioridade *progRodando[maximo_programas]; //Estrutura responsavel pelo controle dos programas pela politica por prioridade

	quantidadeRodando = 0;
	for(loop=0;loop<quantidadeProgramas;loop++){
		if(progPrioridade[loop]->terminado == false){
			quantidadeRodando++;
		}
	}

	nomeMenor = (char*) malloc (sizeof(char) * 15);

	for(loop=0;loop<quantidadeRodando;loop++)
		progRodando[loop] = (ProgramaPrioridade*) malloc (sizeof(ProgramaPrioridade) * 25);

	loop=0;
	loop2=0;
	while(loop<quantidadeProgramas){
		if(progPrioridade[loop]->terminado == false){
			progRodando[loop2] = progPrioridade[loop];
			loop2++;
		}
		loop++;
	}

	strcpy(nomeMenor, progRodando[0]->nome);
	menor = progRodando[0]->prioridade;

	for(loop=0;loop<quantidadeRodando;loop++){

		if(progRodando[loop]->prioridade < menor){
			menor = progRodando[loop]->prioridade;
			strcpy(nomeMenor, progRodando[loop]->nome);
			
		}
	}

	return nomeMenor;
}