
//módulo para manipulação de arquivos e diretórios.


#define BUFSIZE 1024

//função que lê arquivo em path e armazena conteúdo em buf. Lê a partir da posição offset uma quantidade de nrbytes. retorna o número de bytes lidos. Se posição + nrbytes for maior que o tamanho do arquivo, a leitura é truncada(confirmar com os profs).
int le_arquivo(char* path, char *payload, int nrbytes, int offset);

//função que escreve nrbytes do conteúdo em payload no arquivo path a partir da posição offset. Retorna o número de bytes escritos. se offset for maior do que o tamanho do arquivo, então offset é considerado como o último endereço do arquivo. 
int escreve_arquivo(char *path, char *payload, int nrbytes, int offset);

//
