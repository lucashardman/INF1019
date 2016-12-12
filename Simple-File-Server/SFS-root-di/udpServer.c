/* 
 * udpserver.c - A simple UDP echo server 
 * usage: udpserver <port>
 */

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/dir.h> 
#include <sys/param.h> 
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <dirent.h>

#define BUFSIZE 1024

#define TRUE 1
#define FALSE 0

#define MAXFILES 40

extern  int alphasort(); 

int file_select(const struct direct *entry) 
{ 
    if ((strcmp(entry->d_name, ".") == 0) || (strcmp(entry->d_name, "..") == 0))  
        return (FALSE); 
    else 
        return (TRUE); 
}

/*
 * error - wrapper for perror
 */
void error(char *msg) {
	perror(msg);
	exit(1);
}

int remove_folder (char *path, int first, char *buf, char *dirname){

	int empty;
	struct dirent *next_file;
	char filePath[256];
	char dot[256], dotdot[256];
	DIR *folder;
	DIR *home = opendir(".");
	int found = FALSE;
	
	folder = opendir(path);

	empty = rmdir(path);
	if(empty >= 0)
	{
		//escrever buf
		return 0;
	}

	//dot e dotdot guardam o path das pastas /. e /..
	strcpy(dot, path);
	strcat(dot, "/.");
	strcpy(dotdot, path);
	strcat(dotdot, "/..");

	//Este if serve para checar se a pasta path eh existente. Nao se entra neste if nas chamadas recursivas.
	if(first == TRUE){

		while((next_file = readdir(home)) != NULL){

			sprintf(filePath, "%s/%s", path, next_file->d_name);

			if(strcmp(dirname, next_file->d_name) == 0){
				found = TRUE;
				break;
			}
		}
		closedir(home);
		
		
		if(found == FALSE){
			printf("ERRO. Pasta nao existente.\n");
			strcpy(buf, "DR-REP: ERRO. Pasta nao existente.\n");
			return 1;
		}
	}

	if(empty < 0){

		while((next_file = readdir(folder)) != NULL){ //Varre tudo q tem dentro da pasta

			sprintf(filePath, "%s/%s", path, next_file->d_name);
			
			if(strcmp(filePath, dot) != 0 && strcmp(filePath, dotdot) != 0){  //Previne o programa de alterar /. e /..
				
				if(next_file->d_type == DT_DIR){ //Descobre se eh pasta
					printf("%s is a path.\n", filePath);
					empty = rmdir(filePath);
					if(empty < 0){
						
						remove_folder(filePath, FALSE, NULL,next_file->d_name);
						rmdir(filePath);
					}
				}
				else if(next_file->d_type == DT_REG){ //Descobre se eh arquivo

					printf("%s is a file.\n", filePath);
					remove(filePath);
				}
			}
		}
		closedir(folder);
		empty = rmdir(path);

	}
	if(empty != 0){
		printf("Erro ao remover o diretorio.\n");
		strcpy(buf, "DR-REP: ERRO. Nao foi possivel remover o diretorio.\n");
		return 2;
	}

	printf("Diretorio '%s'foi deletado com sucesso.\n", path);

	if(first == TRUE){
		strcpy(buf, "DR-REP: Diretorio '");
		strcat(buf, path);
		strcat(buf, "' foi removido com sucesso.\n");
	}
	return 0;
}

int read_file(char path[], char *buf, int nrbytes, int offset){

	int fd;
	int i,j;
	size_t nbytes;
	ssize_t bytes_read;
	char tempBuf[BUFSIZE];

	if((fd = open(path, O_RDONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)) == -1){
		printf("Este arquivo nao existe.\n");
		strcpy(buf, "RD-REP: ");
		strcat(buf, "Este arquivo nao existe.\n");
		return 0;
	}
	strcpy(buf, "");

	bytes_read = pread(fd, tempBuf, nrbytes, offset);
	bytes_read = pread(fd, tempBuf, bytes_read, offset); //Conserta caso o numero de bytes entrado como parametro seja maior do que o numero de bytes lido

	if(offset > bytes_read){
		strcpy(buf, "RD-REP: ");
		strcat(buf, "Offset maior que o tamanho do arquivo.\n"); 
		close(fd);
		return 0;
	}
	else{
		strcpy(buf, "RD-REP: ");
		strcat(buf, tempBuf);
	}
	
	printf("Numero de bytes: %d\n%s\n", bytes_read, buf);
	close(fd);
	return 0;
}

//cria novo diretorio se ele nao existe
int create_dir(char *path, char *buf)
{
	struct stat st;
		
	if (stat(path, &st) == -1)
	{
		mkdir(path, 0700);
	
		//buf  = DC-­‐REP,path(string),strlen(int)       
		sprintf(buf, "DC-REP, %s, %d", path, strlen(path));
	}
	else
	{
		//buf  = DC-­‐REP,path(string),strlen(int)       
		strcpy(buf, "DC-REP, , 0");
	}
	return 0;
		
}


//função auxiliar que aloca array de strings dada uma string com o separador ,
char** split_buff (char string[], int *n){

	char str[BUFSIZE];
	char *p, **ret ;
	int n_spaces = 0, i;
	char **buf = (char**) malloc(0 * sizeof(char*));

	strcpy(str, string);
	
	p =  strtok (str, ",");

	//Divisao da string np vetor
	while (p != NULL) {
		buf = realloc (buf, sizeof (char*) * (n_spaces+1));

		if (buf == NULL){
			printf("Erro. Falha na alocacao de memoria.\n");
	    	exit (-1); 
		}
		
		buf[n_spaces] = (char*) malloc((strlen(p)+1) * sizeof(char));
		strcpy(buf[n_spaces], p);
		
		n_spaces++;
		
		p = strtok (NULL, ",");
	}
	for(i = 0; i < n_spaces; i++)
	    {
	    	printf("buf[%d]: %s\n", i, buf[i]);
	    }
	printf("return %d\n", n_spaces);
	*n = n_spaces;
	return buf;

}
//função que libera array de string alocado por split_buff()
void free_buf(int n,  char **buf)
{
	int i;
	
	for(i = 0; i < n; i++)
	{
		if(buf[i] != NULL)
		{
			free(buf[i]);
		}
		
	}
	if(buf != NULL)
	{
		free(buf);
	}
	
	return;
}


//função parser que interpreta o comando recebido em buf e o interpreta de acordo
int parse_buff (char *buf, int n, int *cmd, char *name) {
    
    int i, count;
    //numero de informações no array de comandos
    int ni;
    //array de comandos
    char **cmdstr;
    char str[BUFSIZE];
    struct direct **files;
    struct stat st;
    //arrays usados para inicios e fins de nomes de arquivos(DL­‐REQ)
    int *ini, *fim, x;
    
  	//cmdstr = (char*) malloc(BUFSIZE * sizeof(char));
  	//cmdstr2 = (char*) malloc(BUFSIZE * sizeof(char));
    
    //cmdstr = strtok(buf,", ");
    cmdstr = NULL;
    //printf("%s\n", buf);
    printf("split-buf\n");
    cmdstr = split_buff(buf, &ni);
    printf("ni: %d\n", ni);
    
    for(i = 0; i < ni; i++)
    {
    	printf("cmdstr[%d]: %s\n", i, cmdstr[i]);
    }
    
    
	//RD-­‐REQ,path(string),strlen(int),payload(stringvazio),nrbytes(int),offset(int)
	//lê nrbytes do arquivo path a partir da posição offset
	if (strcmp(cmdstr[0], "RD-REQ") == 0)
	{
		count=0;
		while(cmdstr[count] != NULL){
			count++;
		}
		if(count != 4){
			printf("ERRO. %d parametros. Digite  'comando', 'arquivo', 'quantidade de bytes a serem lidos' e 'a partir de qual byte deve comecar a ler.'\n", count);
			return 0;
		}
		read_file(cmdstr[1], buf, atoi(cmdstr[2]), atoi(cmdstr[3]));
	}
	
	
	//WR-­-REQ,path(string),strlen(int),payload(string),nrbytes(int),offset(int)
	//escreve nrbytes do conteúdo de payload no arquivo path a partir da posição offset
	else if (strcmp(cmdstr[0], "WR-REQ") == 0)
	{
		
	}

	
	
	else if (strcmp(cmdstr[0], "FI-REQ") == 0)
	{
		
	}
	
	//DC-­REQ,path(string),strlen(int), dirname(string),strlen(int)
	//cria um novo subdiretório dirname em path
	else if (strcmp(cmdstr[0], "DC-REQ") == 0)
	{
		
		//str = full relative path
		if(strcmp(cmdstr[1], " ") != 0)
		{
			strcpy(str, cmdstr[1]);
			
			strcat(str, "/");
			
			strcat(str, cmdstr[3]);
		}
		else
		{
			printf("here\n");
			strcpy(str, cmdstr[3]);
		}		
		
		printf("str: %s\n", str);
		
		//cria novo diretório em path se diretório já não existe
		create_dir(str, buf);
		
		
	}
	//DR‐REQ,path(string),strlen(int),dirname(string),strlen(int)
	//remove o sub-diretório dirname de path
	else if (strcmp(cmdstr[0], "DR-REQ") == 0)
	{
		//str = full relative path
		if(strcmp(cmdstr[1], " ") != 0)
		{
			strcpy(str, cmdstr[1]);
			
			strcat(str, "/");
			
			strcat(str, cmdstr[3]);
		}
		else
		{
			strcpy(str, cmdstr[3]);
		}
		
		remove_folder(str, TRUE, buf, cmdstr[3]);
	}
	
	//DL­‐REQ,path(string),strlen(int)
	//mostra o nome dos arquivos no diretório path
	
	else if (strcmp(cmdstr[0], "DL-REQ") == 0)
	{
		printf("DL-REQ!\n");
		
		ini = (int*) malloc(MAXFILES * sizeof(int));
		fim = (int*) malloc(MAXFILES * sizeof(int));
		if(ini == NULL || fim == NULL)
		{
			printf("problema de alocação de memória\n");
			exit(0);
		}
		
		if(strcmp(cmdstr[1], " ") == 0)
		{
			strcpy(cmdstr[1], "./");
		}
		
		count = scandir( cmdstr[1], &files, file_select, alphasort);
		printf("count: %d", count);
		//DL­‐REP,allfilenames(string),fstlstpositions(array[40] of struct{int,int}),nrnames(int)
		//retorna os nomes de todos os arquivos/subdiretórios em path em um unico char array, sendo que fstlstpositions[i] indica a posição inicial e final do i-­ésimo nome em allfilenames
		
		strcpy(buf, "DL-REP,");
		for(i = 0, x = 0; i < count;i++)
		{
			sprintf(str, "%s", files[i]->d_name);
			strcat(buf, str);
			ini[i] = x;
			x += strlen(str);
			fim[i] = x - 1;
		}
		strcat(buf, ",");
		for(i = 0; i < count; i++)
		{
			sprintf(str, "%d %d ", ini[i], fim[i]);
			strcat(buf, str);
		}
		
	}
	
	else 
	{
		printf("ERRO. Comando não reconhecido.\n");
		strcpy(buf, "ERRO. Comando não reconhecido.");
		
	}
    
	//cmd e name s~ao mesmo necessarios????
    //cmd = atoi(cmdstr[0]);
    //name = strtok(NULL, "\0");
    
    free_buf(ni, cmdstr); //free_buf causando erro no hostp da main
    
    return 0;
}

int main(int argc, char **argv) {
  	int sockfd; /* socket */
  	int portno; /* port to listen on */
  	int clientlen; /* byte size of client's address */
  	struct sockaddr_in serveraddr; /* server's addr */
  	struct sockaddr_in clientaddr; /* client addr */
  	struct hostent *hostp; /* client host info */
  	char buf[BUFSIZE]; /* message buf */
  	char *hostaddrp; /* dotted decimal host addr string */
  	int optval; /* flag value for setsockopt */
  	int n; /* message byte size */
    
  	char name[BUFSIZE];   // name of the file received from client
  	int cmd;              // cmd received from client
    
	/* 
	 * check command line arguments 
	 */
	if (argc != 2) {
		fprintf(stderr, "usage: %s <port>\n", argv[0]);
		exit(1);
	}
	portno = atoi(argv[1]);

  	/* 
   	 * socket: create the parent socket 
   	 */
  	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  	if (sockfd < 0) 
    	error("ERROR opening socket");

  	/* setsockopt: Handy debugging trick that lets 
   	 * us rerun the server immediately after we kill it; 
   	 * otherwise we have to wait about 20 secs. 
   	 * Eliminates "ERROR on binding: Address already in use" error. 
   	 */
  	optval = 1;
  	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval , sizeof(int));

  	/*
	 * build the server's Internet address
	 */
  	bzero((char *) &serveraddr, sizeof(serveraddr));
  	serveraddr.sin_family = AF_INET;
  	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
  	serveraddr.sin_port = htons((unsigned short)portno);

  	/* 
   	 * bind: associate the parent socket with a port 
   	 */
  	if (bind(sockfd, (struct sockaddr *) &serveraddr, sizeof(serveraddr)) < 0) 
    	error("ERROR on binding");

  	/* 
   	 * main loop: wait for a datagram, then echo it
   	 */
  	clientlen = sizeof(clientaddr);

  	while (1) {

	    /*
	     * recvfrom: receive a UDP datagram from a client
	     */
	    bzero(buf, BUFSIZE);
	    n = recvfrom(sockfd, buf, BUFSIZE, 0, (struct sockaddr *) &clientaddr, &clientlen);
	    if (n < 0)
	    {
	    	error("ERROR in recvfrom");
	    } 
	    
	    parse_buff(buf,n, &cmd, name);
	    
	    /* 
	     * gethostbyaddr: determine who sent the datagram
	     */
	     
	    printf("teste feedback:%s\n", buf);
	    
	    printf("Olha eu aqui!\n");
	    //dá segfault aqui
	    hostp = gethostbyaddr((const char *)&clientaddr.sin_addr.s_addr, sizeof(clientaddr.sin_addr.s_addr), AF_INET);
		
		printf("Olha eu ali!\n");
	    
	    if (hostp == NULL)
	    	error("ERROR on gethostbyaddr");
	    
	    hostaddrp = inet_ntoa(clientaddr.sin_addr);
	    
	    if (hostaddrp == NULL)
	    {
	    	error("ERROR on inet_ntoa\n");
	    }
	      
	    printf("server received datagram from %s (%s)\n", hostp->h_name, hostaddrp);
		printf("aqui!! Oi!!\n");
		printf("buf: %s\n", buf);
	    printf("server received %ld/%d bytes: %s\n", strlen(buf), n, buf);
	    
	    /* 
	     * sendto: echo the input back to the client 
	     */
	    n = sendto(sockfd, buf, strlen(buf), 0, (struct sockaddr *) &clientaddr, clientlen);

	    if (n < 0) 
	    	error("ERROR in sendto");
  	}
}
