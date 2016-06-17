/***************************
 * Lucas Hardman - 1113567 *
 * Cleo Peixoto - 1113657  *
 ***************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "substituicaoPagina.h"

PaginaTabela *criarPaginaTabela (int tamanho){

	int paginaBits = getPaginaBits(tamanho);
	int tamanhoTabela = pow(2, 32 - paginaBits);

	PaginaTabela *paginaTabela = (PaginaTabela*)malloc(tamanhoTabela*sizeof(PaginaTabela));
	if(!paginaTabela){
		printf("Erro na alocacao de memoria para simulacao.\n");
		return NULL;
	}

	memset(paginaTabela, 0, tamanhoTabela * sizeof(PaginaTabela));

	return paginaTabela;
}

int getPaginaBits(int tamanho){
	return log2(tamanho << 10);
}

int *criarVetorPagina (int memoriaTamanho, int paginaTamanho, int *nVetorPagina){

	int *vetorPaginas, loop;

	*nVetorPagina = memoriaTamanho / paginaTamanho;
	printf("Maximo de paginas ao mesmo tempo: %d\n", *nVetorPagina);

	vetorPaginas = (int*) malloc ((*nVetorPagina) * sizeof(int));
	if(!vetorPaginas){
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

int algLRU(PaginaTabela *paginaTabela, int *paginaVetor, int nPaginaVetor){

	int minVetorPaginaNaoAcessadaIndex = -1;
	int minTempoPaginaNaoAcessada;
	int minPaginaVetorIndex = -1;
	int minTempoPagina;

	int loop;

	for(loop=0;loop<nPaginaVetor; ++loop){
		if(loop == 0){
			minPaginaVetorIndex = loop;
			minTempoPagina = paginaTabela[paginaVetor[loop]].ultimoAcesso;
			if(paginaTabela[paginaVetor[loop]].r == 0){
				minVetorPaginaNaoAcessadaIndex = loop;
				minTempoPaginaNaoAcessada = minTempoPagina;
			}
		}
		else if(paginaTabela[paginaVetor[loop]].ultimoAcesso < minTempoPagina){
			minPaginaVetorIndex = loop;
			minTempoPagina = paginaTabela[paginaVetor[loop]].ultimoAcesso;
			if(paginaTabela[paginaVetor[loop]].r == 0){
				minVetorPaginaNaoAcessadaIndex = loop;
				minTempoPaginaNaoAcessada = minTempoPagina;
			}
		}
	}

	if(minVetorPaginaNaoAcessadaIndex != -1)
		return minVetorPaginaNaoAcessadaIndex;
	return minPaginaVetorIndex;
}
int algNRU(PaginaTabela *paginaTabela, int *paginaVetor, int nPaginaVetor){

	int x,y;

	for(y=0; y<4; ++y){
		for(x=0; x<nPaginaVetor; ++x){
			PaginaTabela *pagina = &paginaTabela[paginaVetor[x]];
			if(y == 0 && pagina->r == 0 && pagina->w == 0){
				return x;
			}
			if(y == 1 &&  pagina->r == 0 && pagina->w == 1){
				return x;
			}
			if(y == 2 && pagina->r == 1 && pagina->w == 0){
				return x;
			}
			if(y == 3 && pagina->r == 1 && pagina->w == 1){
				return x;
			}
		}
	}
	return 0;
}
int algSEG(PaginaTabela *paginaTabela, int *paginaVetor, int nPaginaVetor){

	int paginaIndex = paginaVetor[0];
	int loop;

	do{
		for(loop=0;loop<nPaginaVetor; ++loop){
			paginaVetor[loop] = paginaVetor[loop+1];
		}
		paginaVetor[nPaginaVetor-1] = paginaIndex;

		if(paginaTabela[paginaIndex].r == 0){
			return nPaginaVetor-1;
		}
		else{
			paginaTabela[paginaIndex].r = 0;
		}
	} while (1);

}
