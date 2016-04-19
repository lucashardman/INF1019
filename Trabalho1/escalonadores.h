#define maximo_programas 15 //Maximo de programas que podem ser executados simultaneamente

/* Estruturas com os dados lidos no arquivo exec.txt */

typedef struct programaPrioridade{
	char execStr[5];
	char nome[15];
	int prioridade;
}ProgramaPrioridade;

typedef struct programaRoundRobin{
	char execStr[5];
	char nome[15];
}ProgramaRoundRobin;

typedef struct programaLoteria{
	char execStr[5];
	char nome[15];
	int numeroTickets;
}ProgramaLoteria;

/* Funcoes responsaveis pelos algoritmos de escalonamento */

void escalonamentoPorPrioridade(int quantidadeProgramas, ProgramaPrioridade *programas[maximo_programas]);

void escalonamentoRoundRobin(int quantidadeProgramas, ProgramaRoundRobin *programas[maximo_programas]);

void escalonamentoLoteria(int quantidadeProgramas, ProgramaLoteria *programas[maximo_programas]);