
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

	int loop1 = 0, loop2 = 0;; //Variaveis auxiliares para loop
	int pid[maximo_programas]; //Variaveis responsaveis por guardar os pid dos processos
	int timeSharing = 3000000; //Tempo reservado para a execucao de cada programa em microssegundos: 3 segundos
	int *ordem;
	char relatorio[5000]; //Relatorio que sera impresso no saida.txt

	int quantidadeRodando = quantidadeProgramas; //Quantidade de processos em execucao
	float contadorTempo = 0; //Responsavel por medir o tempo que cada programa demora pra executar

	int waitpidResult = 0; //Variavel para guardar os resultados dos retornos da funcao waitpid
	int waitpidStatus = 0; //Variavel para guardar o estado do processo pelo waitpid

	metodoEscalonamento = 1; //Por Prioridade

	/* Inicializar relatorio para saida.txt */
	strcpy(relatorio, "Relatorio:");
	strcat(relatorio, "\n\n");
	/* Fim: inicializa relatorio para saida.txt */

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
	ordem = ordemPrioridade(quantidadeProgramas);

	loop1 = 0;
	loop2 = 0; //Menor prioridade eh 1 e vai ate 7
	while(1){

		if(testaProgramasFinalizados(quantidadeProgramas, &quantidadeRodando) == true){ // Testa se todos os programas ja foram finalizados

			printf("\nFim da execucao de todos programas pela politica de escalonamento por Prioridade\nTempo total de execucao dos progaras: %.2f\n", contadorTempo/1000);
			imprimeRelatorio(relatorio);
			return; //Finaliza a funcao de escalonamento por prioridade
		}

		for(loop2=0;loop2<quantidadeProgramas;loop2++){
			if(ordem[loop1] == progPrioridade[loop2]->prioridade){
				if(progPrioridade[loop2]->terminado == false){

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

						progPrioridade[loop2]->tempoExecucao = contadorTempo;
						kill(pid[loop2], SIGSTOP); //Sinal para o programa entrar em estado de espera

						strcat(relatorio, "Prioridade: ");
						sprintf(relatorio, "%s%d",relatorio, ordem[loop1]);
						strcat(relatorio, " - ");
						strcat(relatorio, progPrioridade[loop2]->nome);
						strcat(relatorio, "\n");
						
					}
					else{ //Fim da execucao de um programa
						progPrioridade[loop2]->tempoExecucao = contadorTempo;
						printf("O programa %s terminou em %.2f segundos.\n", progPrioridade[loop2]->nome, progPrioridade[loop2]->tempoExecucao/1000);
						fflush(stdout);
						progPrioridade[loop2]->terminado = true;

						strcat(relatorio, "Prioridade: ");
						sprintf(relatorio, "%s%d",relatorio, ordem[loop1]);
						strcat(relatorio, " - O programa ");
						strcat(relatorio, progPrioridade[loop2]->nome);
						strcat(relatorio, " terminou em ");
						sprintf(relatorio, "%s%.2f",relatorio, progPrioridade[loop2]->tempoExecucao/1000);
						strcat(relatorio, " segundos");
						strcat(relatorio, "\n\n");		
					}
				}
				else{
					printf("%s ja terminou\n", progPrioridade[loop2]->nome);
				}
			}
		}
			
		loop1++;

		//Condicional responsavel por manter o loop1 rodando entre 0 e a quantidade de programas
		if(loop1 == quantidadeProgramas)
			loop1 = 0;

		fflush(stdout);
	}
	/* Fim do algoritmo de Prioriodade */
}
