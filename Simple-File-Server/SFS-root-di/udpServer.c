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

//tamanho base da meta-informação em um arquivo/diretório padrão. É usada para armazenar um inteiro por escrito, que por sua vez informa qual o tamanho total da meta informação.
#define min_meta 4

#define BUFSIZE 1024

#define TRUE 1
#define FALSE 0

#define MAXFILES 40
#define MAXFILENAME 50

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

//função auxiliar que aloca array de strings dada uma string com o separador ,
char** split_buff (char string[], int *n){

	char str[BUFSIZE];
	char *p;
	int n_spaces = 0;
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
	
	*n = n_spaces;
	return buf;

}

char** split_buff_barra (char string[], int *n){

	char str[BUFSIZE];
	char *p;
	int n_spaces = 0;
	char **buf = (char**) malloc(0 * sizeof(char*));

	strcpy(str, string);
	
	p =  strtok (str, "/");

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
		
		p = strtok (NULL, "/");
	}
	
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


//checa se usuário user tem permissão para acessar arquivo em path. Se não tiver, retorna -1. Se tiver, retorna o tamanho da meta-informação no arquivo. Desired access é 'W' ou 'R'.
int check_permission(char* path, int user, char desired_acess)
{
	int bytes_read, fd;
	char tempBuf[MAXFILENAME];
	int meta_size, i;
	char **infos;
	int n_infos;
	if((fd = open(path, O_RDONLY)) > 0)
	//arquivo existe
	{
		bytes_read = pread(fd, tempBuf, 4,0);
		printf("bytes_read : %d\n", bytes_read);
		i = 0;
		while(tempBuf[i] == '0')
		{
			if(i >= 4)
			{
				printf("Meta-informação inconsistente. i: %d\n", i);
				close(fd);
				return -1;
			}
			i++;
		}
		printf("tempBuf: %s\n", tempBuf);
		printf("tempBuf[i]: %s\n", &tempBuf[i]);
		
		meta_size = atoi(&tempBuf[i]);
		printf("meta_size: %d\n", meta_size);
		pread(fd, tempBuf, meta_size, 0);
		close(fd);
		infos = split_buff(tempBuf, &n_infos);
		if(n_infos != 5 + 1)
		{
			printf("Meta-informação inconsistente. n:infos: %d\n", n_infos);
			free_buf(n_infos, infos);
			return -1;
		}
		else 
		//checa permissões
		{
			if(user == atoi(infos[3]))
			//se usuário é o dono:
			{
				if(infos[4][0] == 'W' || infos[4][0] == desired_acess)
				//permissão concedida
				{
					printf("concedida?\n");
					free_buf(n_infos, infos);
					return meta_size;
				}
				else
				{
					free_buf(n_infos, infos);
					return -1;
				}
			}
			else
			//se usuário não é o dono:
			{
				if(infos[4][1] == 'W' || infos[4][1] == desired_acess)
				//permissão concedida
				{
					free_buf(n_infos, infos);
					return meta_size;
				}
				else
				{
					free_buf(n_infos, infos);
					return -1;
				}
			}
		}
		free_buf(n_infos, infos);
				
	}
	else
	//arquivo não existe
	{
		return -2;
	}
}
int print_permission(char* path, char *buf){

	int bytes_read, fd;
	char tempBuf[MAXFILENAME];
	int meta_size, i;
	char **infos;
	int n_infos;
	
	

	if((fd = open(path, O_RDONLY)) > 0)
	//arquivo existe
	{
		bytes_read = pread(fd, tempBuf, 4,0);
		printf("bytes_read : %d\n", bytes_read);
		i = 0;
		while(tempBuf[i] == '0')
		{
			if(i >= 4)
			{
				printf("Meta-informação inconsistente. i: %d\n", i);
				close(fd);
				return -1;
			}
			i++;
		}

		meta_size = atoi(&tempBuf[i]);
		printf("meta_size: %d\n", meta_size);
		pread(fd, tempBuf, meta_size, 0);
		close(fd);
		infos = split_buff(tempBuf, &n_infos);
		if(n_infos != 5 + 1)
		{
			printf("Meta-informação inconsistente. n:infos: %d\n", n_infos);
			free_buf(n_infos, infos);
			return -1;
		}

		sprintf(buf,"FI-REP,%s,%d,%s,'%s',%s",path, (int) strlen(path), infos[3], infos[4],infos[2]);

		free_buf(n_infos, infos);
				
	}
	else
	//arquivo não existe
	{
		sprintf(buf,"FI-REP,%s,%d,arquivo inexistente",path, (int) strlen(path));
		return -2;
	}
	return 0;
}
//cria entrada em meta-info.txt. Cria meta-info.txt se ele não existe
//path(string),strlen(int),owner(int),permissions(2char)
int create_entry(char* path, int owner, char* permissions)
{
	int fd, w = -1;
	char temp[20];
	char *el = ",\n";
	char *comma = ",";
	if((fd = open(path, O_WRONLY|O_CREAT, 0777)) > 0){
		
		lseek(fd, 0L,SEEK_SET);
		//reserva espaço para meta informação
		w = write(fd, "xxxx,", 5);
		w += write(fd, path, strlen(path));
		//itoa(strlen(path),temp,10);
		w += write(fd, comma, 1);
		snprintf(temp, 20, "%d", (int) strlen(path));
		w += write(fd, temp, strlen(temp));
		w += write(fd, comma, 1);
		//itoa(owner,temp,10);
		snprintf(temp, 20, "%d", owner);
		w += write(fd, temp, strlen(temp));
		w += write(fd, comma, 1);
		w += write(fd, permissions,2);
		w += write(fd,el, strlen(el));
		//escreve tamanho total da meta-informação no início do arquivo
		lseek(fd,0L, SEEK_SET);
		snprintf(temp,20, "%04d", w);
		write(fd, temp, strlen(temp));
		close(fd);
	}
	printf("w: %d\n", w);
	return w;
}

//recebe a path de qualquer coisa e a permissão desejada de retorna >0 se a permissão é concedida, 'R' ou 'W'
int check_dir_permission(char *path, int user, char permission)
{
	int i,n,p;
	char pathdir[50];
	char **div = split_buff_barra(path,&n);
		
	
	strcpy(pathdir, "");
	for(i = 0; i < n-1; i++)
	{
		strcat(pathdir,div[i]);
	}
	strcat(pathdir,"/dir-info.txt");
	printf("pathdir: %s\n", pathdir);
	p = check_permission(pathdir,user, permission);
	if(p > 0)
	{
		//permissão concedida
		free_buf(n,div);
		return 1;
	}
	else
	{
		printf("permissão negada\n");
		free_buf(n,div);
		return -1;
	}
	
}

int check_dir_permission_for_dirs(char *path, int user, char permission)
{
	int p;
	char pathdir[50];
		
	strcpy(pathdir, path);	
	strcat(pathdir,"/dir-info.txt");
	printf("pathdir: %s\n", pathdir);
	p = check_permission(pathdir,user, permission);
	if(p > 0)
	{
		//permissão concedida
		return 1;
	}
	else
	{
		printf("permissão negada\n");
		return -1;
	}
	
}


int remove_folder (char *path, int first, char *buf, char *dirname, int user){

	int empty;
	struct dirent *next_file;
	char filePath[256], str[BUFSIZE];
	char dot[256], dotdot[256];
	DIR *folder;
	int found = 0; // 0 se FALSE, -1 se TRUE
	int ret;
		
	strcpy(str,path);
	
	if(first == TRUE){
		//str = full relative path
		if(strcmp(str, " ") != 0)
		{
			strcpy(path, str);
		
			strcat(path, "/");
		
			strcat(path, dirname);
		}
		else
		{
			strcpy(path, dirname);
		}
	}
	
	ret = check_dir_permission_for_dirs(path, user, 'W');

	if(ret <= 0)
	{
		printf("denied\n");
		strcpy(buf, "DL-REP,denied");
		return 0;
	}
	printf("allowed\n");
	
	folder = opendir(path);

	empty = rmdir(path);
	
	if(empty >= 0)
	{
		sprintf(buf,"DR-REP,%s,%d",str, (int) strlen(str));
		return 0;
	}

	//dot e dotdot guardam o path das pastas /. e /..
	strcpy(dot, path);
	strcat(dot, "/.");
	strcpy(dotdot, path);
	strcat(dotdot, "/..");

	//Este if serve para checar se a pasta path eh existente. Nao se entra neste if nas chamadas recursivas.
	if(first == TRUE){
		
		found = mkdir(path, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

		if(found == 0){
			rmdir(path);
			printf("ERRO. Pasta nao existente.\n");
			sprintf(buf,"RD-REP,%s,0",str);
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
						
						remove_folder(filePath, FALSE, NULL,next_file->d_name,user);
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
		return 2;
	}

	printf("Diretorio '%s'foi deletado com sucesso.\n", path);

	if(first == TRUE){

		sprintf(buf,"RD-REP,%s,%d",str, (int) strlen(str));
	}
	return 0;
}


int read_file(char path[], char *buf, int nrbytes, int offset, int user){

	int fd, mOffset, rdir;
	ssize_t bytes_read;
	char tempBuf[BUFSIZE];
	
	//checa permissão de leitura no diretório
	rdir = check_dir_permission(path, user, 'R');
	if(rdir <= 0)
	{
		printf("Permissão negada no diretório.\n");
		return -1;
	}
	
	//checa permissão de leitura. Se concedida, o retorno é > 0 e igual a valor ao offset da meta informação
	mOffset = check_permission(path, user, 'R');
	//permissão concedida:
	if(mOffset > 0)
	{
		
	}
	else if(mOffset == -1)
	{
		printf("Permissão negada\n");
		return -1;
	}
	else if(mOffset == -2)
	//arquivo não existe.
	{
		
	}
		

	if((fd = open(path, O_RDONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)) == -1){
		printf("Este arquivo nao existe.\n");
		sprintf(buf,"RD-REP,%s,%d, ,0,%d", path, (int) strlen(path), offset);
		return 0;
	}
	strcpy(buf, "");

	bytes_read = pread(fd, tempBuf, nrbytes, offset + mOffset);

	if(bytes_read == 0){

		sprintf(buf,"RD-REP,%s,%d, ,0,%d",path,(int) strlen(path),offset);
		close(fd);
		return 0;
	}
	else if(bytes_read == nrbytes){
		tempBuf[bytes_read+1] = '\0';
		sprintf(buf,"RD-REP,%s,%d,%s,%d,%d",path,(int) strlen(path),tempBuf,(int) bytes_read,offset);
	}
	else if(bytes_read < nrbytes){
		tempBuf[bytes_read] = '\0';
		sprintf(buf,"RD-REP,%s,%d,%s,%d,%d",path,(int) strlen(path),tempBuf,(int) bytes_read,offset);
	}
	
	printf("Numero de bytes: %d\n%s\n", (int) bytes_read, buf);
	close(fd);
	return 0;
}

int write_file(char *path, char *buf, int nrbytes, int offset, char* payload, int user, char* perm)
{
	int fd;
	int w, rdir;
	//offset equivalente a meta-informação dentro do arquivo
	int mOffset;
	
	//checa se arquivo a ser modificado é um dos arquivos do próprio servidor
	if(strcmp(path,"udpServer.c") == 0 || strcmp(path,"meta-info-format.txt") == 0)
	{
		printf("Permissão negada\n");
		return -2;
	}
	
	//checa permissão de escrita no diretório
	rdir = check_dir_permission(path, user, 'W');
	if(rdir <= 0)
	{
		printf("Permissão negada no diretório.\n");
		return -1;
	}
		
	//checa permissão de escrita no arquivo. Se concedida, o retorno é > 0 e igual a valor ao offset da meta informação
	mOffset = check_permission(path, user, 'W');
	if(mOffset > 0)
	{
		
	}
	else if(mOffset == -1)
	{
		printf("Permissão negada\n");
		return -1;
	}
	else if(mOffset == -2)
	//arquivo não existe. Cria arquivo
	{
		//cria arquivo do usuário com as permissões passadas. Retorna o offset da meta-informação
		mOffset = create_entry(path,user, perm);
	}
	else
	{
		printf("Meta-informação inconsistente, mOffset: %d \n", mOffset);
		return -3;
	}
	
	if(nrbytes == 0)
	//deleta arquivo
	{
		printf("deletando %s\n", path);
		unlink(path);		
		return -1;
	}
	
	printf("write\n");
	printf("path: %s, nrbytes: %d, offset %d, payload %s,\n", path, nrbytes,offset,payload);
	
	
	//menor que zero indica não permissão ou erro de criação
	printf("mOffset: %d\n", mOffset);
	if(mOffset > 0)
	{
		if((fd = open(path, O_WRONLY, 0777)) > 0)
		//arquivo já existe
		{		
			lseek(fd, offset + mOffset,SEEK_SET);
			w = write(fd, payload, nrbytes);
			close(fd);
		}
	}
	
	/*
	else
	{
		//cria arquivo
		if((fd = open(path, O_WRONLY|O_CREAT, 0777)) > 0)
		
		{		
			lseek(fd, offset,SEEK_SET);
			w = write(fd, payload, nrbytes);		
			//cria entrada para o novo arquivo na meta-informação
			create_entry(path,1710, "WN");
			close(fd);
		}
	}
	*/
	
	return w;
}

//cria novo diretorio se ele nao existe
int create_dir(char *path, char *buf, int user, char *perms)
{
	struct stat st;
	char npath[50];
		
	if (stat(path, &st) == -1)
	{
		mkdir(path, 0700);
		strcpy(npath,path);
		strcat(npath,"/dir-info.txt");
		create_entry(npath, user, perms);
		
	
		//buf  = DC-­‐REP,path(string),strlen(int)       
		sprintf(buf, "DC-REP, %s, %d", path, (int) strlen(path));
	}
	else
	{
		//buf  = DC-­‐REP,path(string),strlen(int)       
		strcpy(buf, "DC-REP, , 0");
	}
	return 0;
		
}

//função parser que interpreta o comando recebido em buf e o interpreta de acordo
int parse_buff (char *buf, int n, int *cmd, char *name) {
    
    int i, count,w,ret;
    //numero de informações no array de comandos
    int ni;
    //array de comandos
    char **cmdstr;
    char str[BUFSIZE];
    struct direct **files;
    //arrays usados para inicios e fins de nomes de arquivos(DL­‐REQ)
    int *ini, *fim, x;
    
  	//cmdstr = (char*) malloc(BUFSIZE * sizeof(char));
  	//cmdstr2 = (char*) malloc(BUFSIZE * sizeof(char));
    
    //cmdstr = strtok(buf,", ");
    cmdstr = NULL;
    
    cmdstr = split_buff(buf, &ni);
    
    for(i = 0; i < ni; i++)
    {
    	printf("cmdstr[%d]: %s\n", i, cmdstr[i]);
    }
    
    
	//RD-­‐REQ,path(string),strlen(int),payload(stringvazio),nrbytes(int),offset(int),user(int)
	//lê nrbytes do arquivo path a partir da posição offset
	if (strcmp(cmdstr[0], "RD-REQ") == 0)
	{
		count=0;
		while(cmdstr[count] != NULL){
			count++;
		}
		
		if(count < 6){
			printf("ERRO. %d parametros. Digite  'comando', 'arquivo', 'tamanho do nome do arquivo', 'um espaco vazio' ,'quantidade de bytes a serem lidos' e 'a partir de qual byte deve comecar a ler, e o usuário'\n", count);
			
			count=0;
			while(cmdstr[count] != NULL){
				printf("%s\n", cmdstr[count]);
				count++;
			}
			
			return 0;
		}
		read_file(cmdstr[1], buf, atoi(cmdstr[4]), atoi(cmdstr[5]), atoi(cmdstr[6]));
	}
	
	
	//WR-­REQ,path(string),strlen(int),payload(string),nrbytes(int),offset(int),user(int),perms(char[2])
	//escreve nrbytes do conteúdo de payload no arquivo path a partir da posição offset
	else if (strcmp(cmdstr[0], "WR-REQ") == 0)
	{
		count=0;
		while(cmdstr[count] != NULL){
			count++;
		}
		
		
		if(count < 7){
			printf("ERRO. %d parametros. Digite  'comando', 'arquivo', 'quantidade de bytes a serem lidos' e 'a partir de qual byte deve comecar a ler. E usuário e permissões'\n", count);
			
			count=0;
			while(cmdstr[count] != NULL){
				printf("%s\n", cmdstr[count]);
				count++;
			}
			
			return 0;
		}
				
		w = write_file(cmdstr[1], buf, atoi(cmdstr[4]), atoi(cmdstr[5]), cmdstr[3], atoi(cmdstr[6]), cmdstr[7]);
		
		//constrói reply
		strcpy(buf,"");
		sprintf(buf, "WR-REP,%s,%ld, ,%d,%d",cmdstr[1],strlen(cmdstr[1]), w,atoi(cmdstr[5]));
		
	}

	//FI-­REQ,path(string),strlen(int),owner(string),permissions(2 char),filelenght(int)
	//lista os meta-dados dos arquivos
	else if (strcmp(cmdstr[0], "FI-REQ") == 0)
	{
		count=0;
		while(cmdstr[count] != NULL){
			count++;
		}
		
		
		if(count < 3){
			printf("ERRO. %d parametros. Digite  'comando', 'arquivo', 'tamanho da string do arquivo.'\n", count);
			
			count=0;
			while(cmdstr[count] != NULL){
				printf("%s\n", cmdstr[count]);
				count++;
			}
			
			return 0;
		}
		
		print_permission(cmdstr[1],buf);
	}
	
	//DC-­REQ,path(string),strlen(int), dirname(string),strlen(int),user(int),permission(char[2])
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
			strcpy(str, cmdstr[3]);
		}		
		
		printf("str: %s\n", str);
		
		//cria novo diretório em path se diretório já não existe
		create_dir(str, buf, atoi(cmdstr[5]),cmdstr[6]);
		
		
	}
	//DR‐REQ,path(string),strlen(int),dirname(string),strlen(int),user(int)
	//remove o sub-diretório dirname de path
	else if (strcmp(cmdstr[0], "DR-REQ") == 0)
	{
		
		count=0;
		while(cmdstr[count] != NULL){
			count++;
		}

		if(count < 5){
			printf("ERRO. %d parametros. Digite  'comando', 'endereco', 'tamanho da string do endereco', 'arquivo', 'tamanho da string do arquivo' e usuário.\n", count);
			
			count=0;
			while(cmdstr[count] != NULL){
				printf("%s\n", cmdstr[count]);
				count++;
			}
			
			return 0;
		}

		remove_folder(cmdstr[1], TRUE, buf, cmdstr[3],atoi(cmdstr[5]));
	}
	
	//DL­‐REQ,path(string),strlen(int),user(int)
	//mostra o nome dos arquivos no diretório path
	
	else if (strcmp(cmdstr[0], "DL-REQ") == 0)
	{
		printf("DL-REQ!\n");
		
		strcpy(str,cmdstr[1]);
		strcat(str,"/dir-info.txt");
		
		ret = check_dir_permission_for_dirs(cmdstr[1],atoi(cmdstr[3]), 'R');
		if(ret <= 0)
		{
			printf("denied\n");
			strcpy(buf, "DL-REP,denied");
			return 0;
		}
		printf("allowed\n");
		
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
	    n = recvfrom(sockfd, buf, BUFSIZE, 0, (struct sockaddr *) &clientaddr, (socklen_t*restrict)&clientlen);
	    if (n < 0)
	    {
	    	error("ERROR in recvfrom");
	    } 
	    
	    parse_buff(buf,n, &cmd, name);
	    
	    /* 
	     * gethostbyaddr: determine who sent the datagram
	     */
	     
	    printf("Feedback: %s\n", buf);
	    
	    hostp = gethostbyaddr((const char *)&clientaddr.sin_addr.s_addr, sizeof(clientaddr.sin_addr.s_addr), AF_INET);
		
	    if (hostp == NULL)
	    	error("ERROR on gethostbyaddr");
	    
	    hostaddrp = inet_ntoa(clientaddr.sin_addr);
	    
	    if (hostaddrp == NULL)
	    {
	    	error("ERROR on inet_ntoa\n");
	    }
	      
	    printf("server received datagram from %s (%s)\n", hostp->h_name, hostaddrp);

	    printf("server received %ld/%d bytes: %s\n", strlen(buf), n, buf);
	    
	    /* 
	     * sendto: echo the input back to the client 
	     */
	    n = sendto(sockfd, buf, strlen(buf), 0, (struct sockaddr *) &clientaddr, clientlen);

	    if (n < 0) 
	    	error("ERROR in sendto");
  	}
}
