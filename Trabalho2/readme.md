#Simulador de memória virtual (sim-virtual)
  
####Autor:
Lucas Hardman<br/>

####Plataforma:
Linux<br/>
  
####Sobre:
Trabalho de Sistemas de Computacao (INF1019) da PUC-Rio de 2016.1. O trabalho	consiste	em um simulador de memória virtual. O
programa é composto por estruturas de dados e mecanismos de mapeamento de memoria necessários para realizar a paginação, e
implementa três algoritmos de substituição de páginas: Not-Recently-Used (NRU), Least-Recently-Used (LRU) e segunda chance (SEG).
<br/>

####Entrada:
Cada linha do arquiv (.log) conterá um endereço de memória (representação hexa-decimal) acessado seguido das letras R para
leitura e W para escrita.<br/> <br/>
Por exemplo: <br/>
0044e4f8 R <br/>
0044e500 R <br/>
0044e520 R <br/>
0700ff10 R <br/>
2f6965a0 W <br/>
0044e5c0 R <br/>
00062368 R <br/>
<br/>
São fornecidos pelo professor quatro arquivos de entrada, que correspondem a sequências de acesso à memoria de uma 
execução real dos seguintes quatro programas considerados significativos: compilador.log, matriz.log, compressor.log e 
simulador.log, ou seja um compilador, um programa científico, um compressor de arquivos e um simulador de partículas. <br/>

####Saída:
Quando o ultimo acesso à memoria for processado, o simulador deve gerar um pequeno relatório, contendo: <br/>
-> A configuração utilizada (os quatro parâmetros de sim-virtual); <br/>
-> O número total de acessos à memória contidos no arquivo; <br/>
-> O número de page faults (páginas carregadas na memória); <br/>
-> O número de páginas "sujas" que tiveram que ser escritas de volta no disco. <br/>
