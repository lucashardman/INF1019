#define maximo_programas 15 //Maximo de programas que podem ser executados simultaneamente

struct programaPrioridade{
	char execStr[5];
	char nome[15];
	int prioridade;
};

struct programaRoundRobin{
	char execStr[5];
	char nome[15];
};

struct programaLoteria{
	char execStr[5];
	char nome[15];
	int numeroTickets;
};

typedef struct programaPrioridade ProgramaPrioridade;
typedef struct programaRoundRobin ProgramaRoundRobin;
typedef struct programaLoteria ProgramaLoteria;

void escalonamentoPorPrioridade(int quantidadeProgramas, ProgramaPrioridade *programas[maximo_programas]);

void escalonamentoRoundRobin(int quantidadeProgramas, ProgramaRoundRobin *programas[maximo_programas]);

void escalonamentoLoteria(int quantidadeProgramas, ProgramaLoteria *programas[maximo_programas]);

void helloWorld();