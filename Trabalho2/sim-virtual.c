#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct log{
	unsigned int endereco;
	char acesso;
};
typedef struct log Log;

int simulacao(const char* algoritmo, const char* arquivoNome, int paginaTamanho, int memoriaTamanho);
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

	if(strcmp(algoritmo, "LRU") == 0){

	}
	else if(strcmp(algoritmo, "NRU") == 0){

	}
	else if(strcmp(algoritmo, "SEG") == 0){

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
	*logs = carregarLog(arquivoNome, &logQuantidade);
	if(!logs){
		printf("Erro ao carregar o arquivo .log. Tente novamente.\n");
		exit(0);
	}

}

Log *carregarLog(const char* arquivoNome, int* quantidade){

	int c, lines, loop;
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
	lines = ftell(fp) / contadorCaracteres;
	printfd("Numero de linhas no arquivo .log: %d\n", lines);

	*logEntradas = (Log*)malloc(lines * sizeof(Log));
	if(!logEntradas){
		printf("Erro na alocacao de memoria.\n");
		return NULL;
	}

	fseek(fp, 0, SEEK_SET);

	for(loop = 0; loop < lines; ++loop){
		if(fscanf(fp, "%x %c ", &logEntradas[loop].endereco, &logEntradas[loop].acesso) != 2){
			printf("Erro ao carregar o arquivo .log. Certifique-se que cada linha tem o mesmo numero de caracteres");
			break;
		}
	}
	fclose(fp);
	*quantidade = linhas;
	return logEntradas;
}