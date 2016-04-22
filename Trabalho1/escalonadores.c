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

static ProgramaPrioridade *progPrioridade[maximo_programas]; //Variavel responsavel pelo controle dos programas pela politica por prioridade
static ProgramaRoundRobin *progRoundRobin[maximo_programas]; //Variavel responsavel pelo controle dos programas pela politica por round robin
static ProgramaLoteria *progLoteria[maximo_programas]; //Variavel responsavel pelo controle dos programas pela politica por loteria

/****************************** Funcoes estaticas ***************************/
static void iniciarProgramas(int quantidadeProgramas, int *pid);
static bool testaProgramasFinalizados(int quantidadeProgramas, int *quantidadeRodando);
static bool contidoNoVetor(int valor, int *vetor, int tamanho);
static int unsigned sorteioBilhetes();
static void distribuicaoBilhetes(int quantidadeProgramas);

/************************ Funcoes de escalonamento **************************/

/****************************************************************************
 * Politica de escalonamento por prioridade                                 *
 * Parametros:                                                              *
 * quantidadeProgramas - quantidade de programas sendo gerenciados          *
 * *programas - lista de programas sendo gerenciados                        *
 ****************************************************************************/
void escalonamentoPorPrioridade(int quantidadeProgramas, ProgramaPrioridade *programas[maximo_programas]){

	printf("Hello\n");
}

/****************************************************************************
 * Politica de escalonamento Round-Robin                                    *
 * Parametros:                                                              *
 * quantidadeProgramas - quantidade de programas sendo gerenciados          *
 * *programas - lista de programas sendo gerenciados                        *
 ****************************************************************************/
void escalonamentoRoundRobin(int quantidadeProgramas, ProgramaRoundRobin *programas[maximo_programas]){

	int loop1 = 0, loop2 = 0; //Variaveis auxiliares para loop
	int pid[maximo_programas]; //Variaveis responsaveis por guardar os pid dos processos
	float timeSharing = 5; //Tempo reservado para a execucao de cada programa

	int quantidadeRodando = quantidadeProgramas;
	int contadorTempo = 0;

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

	for(loop1=0;loop1<quantidadeProgramas;loop1++){
		printf("O programa: %s de pid %d foi iniciado.\n", progRoundRobin[loop1]->nome, pid[loop1]);
	}

	printf("\n");

	/* Inicio do algoritmo de Round-Robin */
	loop1 = 0;
	loop2 = 0;
	while(1){

		if(testaProgramasFinalizados(quantidadeProgramas, &quantidadeRodando) == true){ // Testa se todos os programas ja foram finalizados

			printf("\nFim da execucao de todos programas pela politica de escalonamento Round-Robin\nTempo total de execucao dos progaras: %d\n", contadorTempo);
			return; //Finaliza a funcao de Round-Robin
		}
		/* Conta quanto tempo esta se passando ao longo da execucao dos programas */
		contadorTempo = contadorTempo + 1;
		
		if(progRoundRobin[loop1]->terminado == false){

			kill(pid[loop1], SIGCONT); //Sinal para o programa entrar em estado de execucao
			fflush(stdout);
			
			sleep(1);

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

			if(waitpidResult == 0){

				loop2++;
				if(loop2 == timeSharing){ //Tempo reservado para a execucao de cada programa

					printf("\nCota de tempo esgotada. Quantidade de programas em execucao: %d\n", quantidadeRodando);
					contadorTempo = contadorTempo - 1; //Esta mensagem nao deve ser contada como tempo de execucao dos programas
					
					kill(pid[loop1], SIGSTOP); //Sinal para o programa entrar em estado de espera

					loop1++;
					loop2 = 0;
				}
			}
			else{ //Fim da execucao de um programa
				contadorTempo = contadorTempo - 1; //Esta mensagem nao deve ser contada como tempo de execucao dos programas
				progRoundRobin[loop1]->tempoExecucao = contadorTempo;

				printf("O programa %s terminou em %d segundos.\n", progRoundRobin[loop1]->nome, progRoundRobin[loop1]->tempoExecucao);
				fflush(stdout);
				progRoundRobin[loop1]->terminado = true;
				loop2 = 0;
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
 * Parametros:                                                              *
 * quantidadeProgramas - quantidade de programas sendo gerenciados          *
 * *programas - lista de programas sendo gerenciados                        *
 ****************************************************************************/
void escalonamentoLoteria(int quantidadeProgramas, ProgramaLoteria *programas[maximo_programas]){

	int loop1 = 0; //Variaveis auxiliares para loop
	int pid[maximo_programas]; //Variaveis responsaveis por guardar os pid dos processos

	/* Inicializacao de progLoteria e sorteio de bilhetes */
	for(loop1=0;loop1<quantidadeProgramas;loop1++){
		progLoteria[loop1] = programas[loop1];
	}
	sorteioBilhetes();
	distribuicaoBilhetes(quantidadeProgramas);
	/* Fim: Inicializacao de progLoteria e sorteio de bilhetes */

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

	for(loop1=0;loop1<quantidadeProgramas;loop1++){
		printf("O programa: %s de pid %d foi iniciado.\n", progLoteria[loop1]->nome, pid[loop1]);
	}

	printf("\n");
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

	//progRoundRobin[0]->terminado = true;

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
 * Descricao: responsavel pela distriguicao de bilhetes para os programas   *
 * que serao executados para o escalonamento por loteria                    *
 * Parametros:                                                              *
 * quantidadeProgramas - quantidade de programas sendo gerenciados          *
 ****************************************************************************/
static void distribuicaoBilhetes(int quantidadeProgramas){

	int loop1 = 0, loop2 = 0, loop4 = 0;;
	bool loop3 = false;
	int maximoBilhetes = 0;
	int sorteado = 100;

	bool bilhetesDiferentes = false; //Variavel para controlar um loop responsavel por nao haver bilhetes repetidos distribuidos entre os programas

	for(loop1=0;loop1<quantidadeProgramas;loop1++){
		maximoBilhetes = maximoBilhetes + progLoteria[loop1]->quantidadeBilhetes;
	}
	if(maximoBilhetes>20){ //Se a quantidade de bilhetes lidos de exec.txt for superior a 20 o programa termina
		printf("Erro no arquivo exec.txt. A quantidade maxima de bilhetes eh 20.\nQuantidade: %d\n", maximoBilhetes);
		exit(1);
	}

    //Inicializando todos bilhetes com o numero 100, para nao ter confusao com numeros no intervalo [0,19]
	for(loop1=0;loop1<quantidadeProgramas;loop1++){ //Loop para cada programa
		for(loop2=0;loop2<progLoteria[loop1]->quantidadeBilhetes;loop2++){ //Para cada bilhete de cada programa
			progLoteria[loop1]->bilhetes[loop2] = 100;
		}
	}

	//Sorteio dos bilhetes
	for(loop1=0;loop1<quantidadeProgramas;loop1++){ //Loop para cada programa
		for(loop2=0;loop2<progLoteria[loop1]->quantidadeBilhetes;loop2++){ //Para cada bilhete de cada programa
			
			loop3 = true;
			while(loop3 == true){
				sorteado = sorteioBilhetes(); //Sorteia os bilhetes
				if(contidoNoVetor(sorteado, progLoteria[loop1]->bilhetes, progLoteria[loop1]->quantidadeBilhetes) == false){
				
					progLoteria[loop1]->bilhetes[loop2] = sorteado; //Sorteia os bilhetes	
					loop3 = false;		
				}
				else{
					loop3 = true;
				}
			}
		}
	}

	/* Print para teste */
	for(loop1=0;loop1<quantidadeProgramas;loop1++){ //Loop para cada programa

		printf("Bilhetes do %s: {", progLoteria[loop1]->nome);
		for(loop2=0;loop2<progLoteria[loop1]->quantidadeBilhetes;loop2++){ //Para cada bilhete de cada programa

			printf("%d", progLoteria[loop1]->bilhetes[loop2]);

			if(loop2 != progLoteria[loop1]->quantidadeBilhetes-1)
				printf(", ");
		}
		printf("}\n");
	}

}

/****************************************************************************
 * Nome: sorteioBilhete                                                     *
 * Descricao: responsavel pelo sorteio de bilhetes.                         * 
 * Condicoes de retorno:                                                    *
 * bilhete - inteiro de 0 a 19 que reprenta um bilhete.                     *
 ****************************************************************************/
static unsigned int sorteioBilhetes(){

	long bilhete;
	int limite = 20;

	unsigned long num_bins = (unsigned long) limite +1;
	unsigned long num_rand = (unsigned long) RAND_MAX +1;
	unsigned long bin_size = num_rand / num_bins;
	unsigned long defect = num_rand % num_bins;

	do{
		bilhete = random();
	}while(num_rand - defect <=(unsigned long) bilhete);

	return (int) bilhete/bin_size;
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
