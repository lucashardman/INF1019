Alunos:
Andre Mazal Krauss - 1410386
Lucas Hardman Gomes Campos Franca - 1113567

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

Comunicação Cliente-Servidor: o seguinte protocolo de comunicação foi implementado, de acordo com o enunciado do trabalho(Simple-File-Server.pdf).
Pares requisição(REQ) - reply(REP)

DL­‐REQ,path(string),strlen(int),user(int)
DR‐REQ,path(string),strlen(int),dirname(string),strlen(int),user(int)
DC-­REQ,path(string),strlen(int), dirname(string),strlen(int),user(int),permission(char[2])
FI-­REQ,path(string),strlen(int),owner(string),permissions(2 char),filelenght(int)
WR-­REQ,path(string),strlen(int),payload(string),nrbytes(int),offset(int),user(int),perms(char[2])
RD-­‐REQ,path(string),strlen(int),payload(stringvazio),nrbytes(int),offset(int),user(int)

Para implementar tudo o que é relacionado às meta-informações do servidor, ou seja, quais arquivos pertencem a quais usuários e quais as permissões de acesso, usamos os primeiros X bytes em cada arquivo, ou então um .txt para cada diretório. Nos arquivos, os X primeiros bytes guardam nome do arquivo, dono, permissões de acesso e o valor de X, que é decidido dinâmicamente de acordo com o tamanho necessário para as outras informações. Nos diretórios, as mesmas informações são guardadas em um arquivo chamado dirname/dir-info.txt, onde dirname é o diretório. O formato específico do armazenamento dessas informações pode ser encontrado em meta-info-format.txt. Antes da realização de qualquer operação de leitura/escrita e destruição de arquivos ou diretórios, o servidor usa essas informações para checar se a operação é legal. As permissões podem ser para o próprio usuário ou para outros, e cada campo desses ocupa um char. Ambos serão sempre: 'W' - indica permissão de escrita E leitura. 'R' - indica permissão de leitura. 'N' - indica não conceder permissões. 
