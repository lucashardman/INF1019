#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "substituicaoPagina.h"

PaginaTabela *criarPaginaTabela (int tamanho){

	int paginaBits = getPaginaBits(tamanho);
	int tamanhoTabela = pow(2, 32 - paginaBits);

	printf("Entradas: %d\n", tamanhoTabela);

	PaginaTabela *paginaTabela = (PaginaTabela*)malloc(tamanhoTabela*sizeof(PaginaTabela));
	if(!paginaTabela){
		printf("Erro na alocacao de memoria para simulacao.\n");
		return NULL;
	}

	memset(paginaTabela, 0, tamanhoTabela * sizeof(PaginaTabela));

	return paginaTabela;
}

getPaginaBits(int tamanho){
	return log2(tamanho << 10);
}

int *criarVetorPagina (int memoriaTamanho, int paginaTamanho, int *nVetorPagina){

	int *vetorPaginas, loop;

	*nVetorPagina = MemoriaTamanho / paginaTamanho;
	printf("Maximo de paginas ao mesmo tempo: %d\n", *nVetorPagina);

	vetorPaginas = (int*) malloc ((*nVetorPagina) * sizeof(int));
	if(!=vetorPaginas){
		printf("Erro na alocacao de memoria para o vetor de paginas.\n");
		return NULL;
	}
	for(loop=0; loop<*nVetorPagina; ++loop){
		vetorPaginas[loop] = -1;
	}

	return vetorPaginas;
}

void atualizaPaginas (PaginaTabela * paginaTabela, int *paginaVetor, int nPaginaVetor){
	
	int loop, index;
	
	for(loop=0;loop<nPaginaVetor;++loop){

		index = paginaVetor[loop];
		if(index != -1){
			paginaTabela[index].r = 0;
		}
	}
}