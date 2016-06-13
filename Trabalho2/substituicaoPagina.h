struct paginaTabela{
	int ultimoAcesso;
	char r;
	char w;
};
typedef struct paginaTabela PaginaTabela;

PaginaTabela *criarPaginaTabela (int tamanho);

int getPaginaBits(int tamanho);

int *criarVetorPagina (int memoriaTamanho, int paginaTamanho, int *nVetorPagina);

void atualizaPaginas (PaginaTabela * paginaTabela, int *paginaVetor, int nPaginaVetor);

int algLRU(PaginaTabela *paginaTabela, int *paginaVetor, int nPaginaVetor);
int algNRU(PaginaTabela *paginaTabela, int *paginaVetor, int nPaginaVetor);
int algSEG(PaginaTabela *paginaTabela, int *paginaVetor, int nPaginaVetor);