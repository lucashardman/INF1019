#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "substituicaoPagina.h"

struct log{
	unsigned int endereco;
	char acesso;
};
typedef struct log Log;

void simulacao(const char* algoritmo, const char* arquivoNome, int paginaTamanho, int memoriaTamanho);
Log *carregarLog(const char* arquivoNome, int* quantidade);

int main (int argc, const char *argv[]){

	if(argc < 5){
		printf("Erro!\n"
			   "Digite no minimo 4 parametros:\n"
			   "1) O algoritmo de substituicao (LRU/NRU/SEG)\n"
			   "2) O arquivo .log contendo a sequencia de enderecosde memoria acessados\n"
			   "3) O tamanho de cada pagina/quadro de pagina em KB (8 a 32 KB)\n"
			   "4) O tamanho total de memoria fisica hipoteticamente disponivel em KB (128KB a 16MB)\n");
		exit(0);
	}

	/*******************************************************
	 * Aqui começa a simulação do sim-virtual.             *
	 * Obs: funcao atol converte uma string em um long int *
	 *******************************************************/
	simulacao(argv[1], argv[2], atol(argv[3]), atol(argv[4]));

	return 0;
}

void simulacao(const char* algoritmo, const char* arquivoNome, int paginaTamanho, int memoriaTamanho){

	int logQuantidade;
	Log *logs;

	PaginaTabela *paginaTabela;
	PaginaTabela *ultimaPagina;

	int nPaginaVetor;
	int *paginaVetor;

	int pageFaults = 0;
	int paginasEscritas = 0;
	int tempo;
	Log logEntrada;
	int paginaIndex;
	int reporIndex;

	int loop;
	int encontrou;

	int paginaBits = getPaginaBits(paginaTamanho);

	//void (*atualiza)(PaginaTabela*, int*, int);
	//int (*alg)(PaginaTabela*, int*, int);

	if(strcmp(algoritmo, "LRU") == 0){
		//alg = algLRU;
	}
	else if(strcmp(algoritmo, "NRU") == 0){
		//atualiza = atualizaPaginas;
		//alg = algNRU;
	}
	else if(strcmp(algoritmo, "SEG") == 0){
		//alg = algSEG;
	}
	else{
		printf("Algoritmo invalido. Tente novamente.\n");
		exit(0);
	}

	if(paginaTamanho < 8 || paginaTamanho > 32){
		printf("Tamanho de pagina invalido: %dKB. Tente novamente com um valor entre 8KB e 32KB.\n", paginaTamanho);		
		exit(0);
	}

	if(memoriaTamanho < 128 || memoriaTamanho > 16384){
		printf("Tamanho de memoria fisica invalido: %dKB. Tente novamente com um valor entre 128KB e 16384KB\n", memoriaTamanho);
		exit(0);
	}

	//Leitura do Log:
	logs = carregarLog(arquivoNome, &logQuantidade);
	if(!logs){
		printf("Erro ao carregar o arquivo .log. Tente novamente.\n");
		exit(0);
	}

	paginaTabela = criarPaginaTabela(paginaTamanho);
	if(!paginaTabela){
		printf("Erro ao criar a tabela de paginas.\n");
		exit(0);
	}

	paginaVetor = criarVetorPagina(memoriaTamanho, paginaTamanho, &nPaginaVetor);
	if(!paginaVetor){
		printf("Erro ao criar o vetor de paginas.\n");
		exit(0);
	}

	printf("Rodando o simulador...\n");
	printf("Arquivo lido: %s.\n", arquivoNome);
	printf("Tamanho da memoria fisica: %dKB.\n", memoriaTamanho);
	printf("Tamanho da pagina: %dKB.\n", paginaTamanho);
	printf("Algoritmo de troca de pagina: %s.\n", algoritmo);

	/***** Inicio do algoritmo do simulador *****/
	for(tempo=0; tempo<logQuantidade; ++tempo){
		
		logEntrada = logs[tempo];
		paginaIndex = logEntrada.endereco >> paginaBits;

		if(tempo % 100 ==  0){
			if(strcmp(algoritmo, "NRU") == 0){
				atualizaPaginas(paginaTabela, paginaVetor, nPaginaVetor);
			}
		}

		encontrou = 0;
		for(loop=0; loop<nPaginaVetor; ++loop){
			if(paginaVetor[loop] == paginaIndex){
				encontrou = 1;
				break;
			}
		}
		if(!encontrou){
			for(loop=0; loop<nPaginaVetor; ++loop){
				if(paginaVetor[loop] == -1){
					paginaVetor[loop] = paginaIndex;
					encontrou = 1;
					break;
				}
			}
			if(!encontrou){
				++pageFaults;
				if(strcmp(algoritmo, "SEG") == 0){
					reporIndex = algSEG(paginaTabela, paginaVetor, nPaginaVetor);
				}
				if(strcmp(algoritmo, "LRU") == 0){
					reporIndex = algLRU(paginaTabela, paginaVetor, nPaginaVetor);
				}
				if(strcmp(algoritmo, "NRU") == 0){
					reporIndex = algNRU(paginaTabela, paginaVetor, nPaginaVetor);
				}
				ultimaPagina = &paginaTabela[paginaVetor[reporIndex]];
				if(ultimaPagina->w){
					++paginasEscritas;
				}
				ultimaPagina->r = 0;
				ultimaPagina->w = 0;

				paginaVetor[reporIndex] = paginaIndex;
			}
		}

		paginaTabela[paginaIndex].ultimoAcesso = tempo;
		if(logEntrada.acesso == 'R'){
			paginaTabela[paginaIndex].r = 1;
		}
		else{
			paginaTabela[paginaIndex].w = 1;
		}
	}
	printf("Page faults: %d.\n", pageFaults);
	printf("Paginas escritas: %d.\n", paginasEscritas);
}

Log *carregarLog(const char* arquivoNome, int* quantidade){

	int c, linhas, loop;
	int contadorCaracteres = 0;

	Log *logEntradas;

	FILE *fp = fopen (arquivoNome, "r");
	if(!fp){
		printf("Nao foi possivel carregar o arquivo .log %s.\n", arquivoNome);
		return NULL;
	}

	//Numero de entradas. Cada linha tem o mesmo numero de caracteres.
	while((c = fgetc(fp)) != EOF){
		++contadorCaracteres;
		if(c == '\n'){
			break;
		}
	}

	fseek(fp, 0, SEEK_END);
	linhas = ftell(fp) / contadorCaracteres;
	printf("Numero de linhas no arquivo .log: %d\n", linhas);

	logEntradas = (Log*)malloc(linhas * sizeof(Log));
	if(!logEntradas){
		printf("Erro na alocacao de memoria.\n");
		return NULL;
	}

	fseek(fp, 0, SEEK_SET);

	for(loop = 0; loop < linhas; ++loop){
		if(fscanf(fp, "%x %c ", &logEntradas[loop].endereco, &logEntradas[loop].acesso) != 2){
			printf("Erro ao carregar o arquivo .log. Certifique-se que cada linha tem o mesmo numero de caracteres");
			break;
		}
	}
	fclose(fp);
	*quantidade = linhas;
	return logEntradas;
}