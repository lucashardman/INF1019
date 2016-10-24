/**************************************************
 * Alunos:                                        *
 * Lucas Hardman Gomes Campos Franca - 1113567    *
 * Andre Mazal Krauss - 1410386                   *
 **************************************************/

--> Compilacao:

gcc -o programa1 programa1.c
gcc -o programa2 programa2.c
gcc -o programa3 programa3.c
gcc -o programa4 programa4.c
gcc -o programa5 programa5.c
gcc -o real-time real-time.c
gcc -o round-robin round-robin.c
gcc -o prioridades prioridades.c
gcc -o interpretador interpretador.c

--> Execucao: ./interpretador

Eh necessario que exista um arquivo entrada.txt no formato do escalonamento desejado.

Digite 1 para escalonar pro prioridades.
Neste caso o arquivo entrada.txt deve ter linhas no formato: "Exec programa INT", onde INT representa a prioridade e deve ser um inteiro de 1 a 7.

Digite 2 para escalonar por round-robin.
Neste caso o arquivo entrada.txt deve ter linhas no formato: "Exec programa".

Digite 3 para escalonar por real-time.
Neste caso o arquivo entrada.txt deve ter linhas no formato: "Exec programa I=INT1 D=INT2", onde INT1 representa o segundo de inicio e INT2 o tempo de duracao.

--> Explicacao (O que realmente esta acontecendo?):

Nesta aplicacao temos 2 processos ocorrendo simultaneamente, o interpretador e um escalonador. Estes processos se comunicam atraves de uma FIFO, onde o
interpretador le a cada 1 segundo uma linha do arquivo entrada.txt e envia os dados lidos para o escalonador. Logo apos ler cada linha, o interpretador envia
um sinal SIGUSR1 para que o escalonador "carregue" as informacoes enviadas pela FIFO.

O handler para o sinal SIGUSR1 eh basicamente o mesmo para todos os escalonadores e serve para carregar os dados enviados pela FIFO. Ao receber o sinal, o processo le a fifo
atraves da funcao 'read' e coloca as informacoes lidas em uma lista de struct. Esta struct eh diferente para cada tipo de escalonamento, e possui as seguintes informacoes:

Prioridades:
> Nome do programa
> Prioridade
> Pid
> Booleano se terminou ou nao

Round-robin:
> Nome do programa
> Pid
> Booleano se terminou ou nao

Real-time:
> Nome do programa
> Segundo de inicio
> Tempo de duracao
> Pid

Apos alocar as informacoes na struct, o handler inicia o processo atraves da funcao execve e logo depois o coloca em estado de espera atraves do signal SIGSTOP.

Agora que o programa esta carregado, o handler incrementa o contador de programas "qtd" e envia uma mensagem avisando que esta carregado.

****
Escalonamento por prioridades:

Este processo entra em um loop repetitivo de i=0 ate 7, onde em cada volta ele escalona os programas de prioridade igual a i. Cada programa com a mesma prioridade tem uma fatia de tempo de 3 segundos para executar e depois passa a vez para o proximo. Quando todos os programas com a mesma prioridade terminam de executar, passa para os programas com a proxima menor prioridade.

Este loop se repete ate que todos os programas escalonados tenham terminado. Esta repeticao eh necessaria pois o interpretador envia os programas de 1 em 1 segundo, por isso eh possivel que um programa de prioridade mais alta seja enviado depois que um programa de prioridade mais baixa ja esteja sendo executado. Neste caso o programa continua a executar ate acabar sua fatia de tempo e depois da a vez para o programa de prioridade maior.

OBS: maior prioridade = 1, menor = 7.

***
Escalonamento round-robin:

No escalonamento round-robin todos os programas possuem a mesma fatia de tempo para executar e sao executados na ordem que o interpretador envia para o escalonador.

****
Escalonamento real-time:

O escalonamento real-time se baseia num vetor de sessenta posições, uma cada para cada segundo no minuto, e num loop que verifica através desse vetor qual programa deve estar sendo executado no momento. Quando um programa é enviado ao escalonador, ele verifica se intervalo de tempo desejado já está tomado. Se não, o programa é adicionado ao vetor. Quando o escalonador percebe que um programa terminou sua execução, suas posições no vetor são libertas para uso por outros programas.

--> Testes:

Temos 5 programas:
programa1 - IO-BOUND - Duracao 14 segundos
programa2 - IO-BOUND - Duracao 19 segundos
programa3 - CPU-BOUND - Duracao 9 segundos
programa4 - CPU-BOUND - Duracao 24 segundos
programa5 - CPU-BOUND - Duracao 5 segundos

***
Teste Prioridades:

entrada.txt

Exec programa1 7
Exec programa2 2
Exec programa3 2
Exec programa4 1
Exec programa5 5

Ordem de execucao determinada pelo escalonador (levando em conta que todos os programas ja esta carregados, ou seja, lidos do entrada.txt pelo interpretador, executados e coloca-dos em estado de espera):

programa4
fim: programa4
programa2
programa3
programa2
programa3
programa2
programa3
fim: programa3
fim: programa2
programa5
fim: programa5
programa1
fim: programa1


Teste round-robin:

entrada.txt

Exec programa1
Exec programa2
Exec programa3
Exec programa4
Exec programa5

Ordem de execucao determinada pelo escalonador (levando em conta que todos os programas ja esta carregados, ou seja, lidos do entrada.txt pelo interpretador, executados e coloca-dos em estado de espera):

programa1
programa2
programa3
programa4
programa5
programa1
programa2
programa3
programa4
programa5
fim: programa5
programa1
programa2
programa3
fim: programa3
programa4
programa1
programa2
programa4
programa1
fim: programa1
programa2
programa4
programa2
programa4
programa2
fim: programa2
programa4
fim: programa4

Teste real-time:

Exec programa1 I=10 D=20
Exec programa2 I=0 D=7
Exec programa3 I=30 D=5
Exec programa4 I=40 D=17
Exec programa5 I=10 D=10
Exec programa4 I=50 D=20

Ordem de execucao determinada pelo escalonador (levando em conta que todos os programas ja esta carregados, ou seja, lidos do entrada.txt pelo interpretador, executados e coloca-dos em estado de espera):
obs: reparar que programa4 I=50 D=20 e programa5 I=10 D=10 não serão escalonados por terem parâmetros incompatíveis ou já utilizados por outro programa. 


começando programa2
Este programa dura 20 segundos.
Programa2 rodando. Faltam 19 segundos.
Programa2 rodando. Faltam 18 segundos.
Programa2 rodando. Faltam 17 segundos.
interrompendo programa2
começando programa1
Este programa dura 15 segundos.
Programa1 rodando. Faltam 14 segundos.
Programa1 rodando. Faltam 13 segundos.
Programa1 rodando. Faltam 12 segundos.
Programa1 rodando. Faltam 11 segundos.
Programa1 rodando. Faltam 10 segundos.
Programa1 rodando. Faltam 9 segundos.
Programa1 rodando. Faltam 8 segundos.
Programa1 rodando. Faltam 7 segundos.
Programa1 rodando. Faltam 6 segundos.
Programa1 rodando. Faltam 5 segundos.
Programa1 rodando. Faltam 4 segundos.
Programa1 rodando. Faltam 3 segundos.
Programa1 rodando. Faltam 2 segundos.
Programa1 rodando. Faltam 1 segundos.
Programa1 finalizado.
programa terminou
começando programa3
interrompendo programa3
começando programa4
interrompendo programa4
começando programa2
Programa2 rodando. Faltam 16 segundos.
Programa2 rodando. Faltam 15 segundos.
Programa2 rodando. Faltam 14 segundos.
Programa2 rodando. Faltam 13 segundos.
Programa2 rodando. Faltam 12 segundos.
Programa2 rodando. Faltam 11 segundos.
Programa2 rodando. Faltam 10 segundos.
interrompendo programa2
começando programa3
programa terminou
começando programa4
programa terminou
começando programa2
Programa2 rodando. Faltam 9 segundos.
Programa2 rodando. Faltam 8 segundos.
Programa2 rodando. Faltam 7 segundos.
Programa2 rodando. Faltam 6 segundos.
Programa2 rodando. Faltam 5 segundos.
Programa2 rodando. Faltam 4 segundos.
Programa2 rodando. Faltam 3 segundos.
interrompendo programa2
começando programa2
Programa2 rodando. Faltam 2 segundos.
Programa2 rodando. Faltam 1 segundos.
Programa2 finalizado.
programa terminou
Fim do escalonamento.

