#Interpretador de Comandos e Escalonador de Programas
  
 ####Autor:
  Lucas Hardman<br/>
  
  ####Sobre:
  Trabalho de Sistemas de Computacao (INF1019) da PUC-Rio de 2016.1. O trabalho	consiste	em programas	que	implementem um
  interpretador de comandos e um escalonador de programas. O interpretador devera solicitar ao escalonador a execucao de 
  programas. O escalonador por sua vez dispara a execucao de programas de acordo com uma determinada politica de escalonamento,
  que devera ser por prioridade, por loteria ou por round-robin.
 
 ####Compilacao e execucao:<br/>
 gcc programa1.c -o programa1<br/>
 gcc programa2.c -o programa2<br/>
 gcc programa3.c -o programa3<br/>
 gcc programa4.c -o programa4<br/>
 gcc main.c escalonadores.c -o trabalho1<br/>
 ./trabalho1<br/>
 -> Digitar 1 para executar escalonamento por prioridade<br/>
 -> Digitar 2 para executar escalonamento por round-robin<br/>
 -> Digitar 3 para executar escalonamento por loteria<br/>
 
 ####Entrada:
 -> Digitar 1, 2 ou 3 para escolher respectivamente as politicas de escalonamento por prioridade, round-robin ou loteria<br/>
 -> Arquivo entrada.txt. O arquivo pode ter 3 formatos diferentes dependendo da politica de escalonamento:<br/>
 1) Por prioridade:<br/>
 Exec \<nome_do_programa\> \<prioridade\><br/>
 onde prioridade eh um numero de 1 a 7<br/>
 2) Round-Robin:<br/>
 Exec \<nome_do_programa\><br/>
 3) Por loteria:<br/>
 Exec \<nome_do_programa\> \<numero_de_bilhetes_a_receber\><br/>
 onde o numero de bilhetes a receber deve ser maior que zero e a soma dos numeros de bilhetes a receber deve ser igual ou inferior a 20<br/>
 
 ####Saida:
 -> Mensagens no prompt de comando sobre o andamento do escalonamento<br/>
 -> Arquivo saida.txt com ordem de execucao dos programas sendo escalonados, mostrando como ocorre a preempcao na execucao dos processos<br/>