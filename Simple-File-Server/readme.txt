Alunos:
Andre Mazal Krauss - 1410386
Lucas Hardman Gomes Campos Francao - 1113567

Estrutura:

udpClient.c
SFS-root-di
	udpServer.c
	arquivos
	pastas

Existem dois processos, o server (udpServer.c) e o client (udpClient.c).....................


Compilacao:
gcc -o client udpClient.c
gcc -o server udpServer.c

Execucao:

Terminal 1:
./server 3999 &

Terminal 2:
./client localhost 3999

DL­‐REQ,path(string),strlen(int),user(int)
DR‐REQ,path(string),strlen(int),dirname(string),strlen(int),user(int)
DC-­REQ,path(string),strlen(int), dirname(string),strlen(int),user(int),permission(char[2])
FI-­REQ,path(string),strlen(int),owner(string),permissions(2 char),filelenght(int)
WR-­REQ,path(string),strlen(int),payload(string),nrbytes(int),offset(int),user(int),perms(char[2])
RD-­‐REQ,path(string),strlen(int),payload(stringvazio),nrbytes(int),offset(int),user(int)