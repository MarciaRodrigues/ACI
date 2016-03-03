
//TRABALHO1 
//Servidor_aplicacao

#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MYPORT 5000
#define BUFF_LENG 1024

   int s;
    int socket_cliente;
    struct sockaddr_in servidor; //guardar inf que preciso relativamente ao servidor, porta...
    char buff[BUFF_LENG]; // quando recebo a mensagem vai ser guardada nesta variavel
	
int Connect (server_add, port)
{
 
    
    servidor.sin_family = AF_INET; //AF_INET - dominio do socket (host e porta)
    servidor.sin_addr.s_addr = INADDR_ANY; // servidor pode receber de qualquer endereço
    servidor.sin_port = htons (MYPORT); //porta
    
    s = socket(PF_INET, SOCK_STREAM,0); // sock_stream - circuito virtual
    
    if (s== -1)  //para ver se o socket foi criado com sucesso
            {
                printf("ERRO");
                return -1;
            }
    puts("BEM SUCEDIDO");

//bind - assiciar um socket a um enderço
    
    bind(s,(struct sockaddr *) &servidor, sizeof(servidor));
    
//listen - numero de ligaçoes que o teu servidor consegue manter max 5
    
    listen(s,5);
    
    socket_cliente = accept (s, NULL, NULL);
    
        if (socket_cliente<0)
            
        {
            printf("ERRO");
            
            return -1;
        }
		
		
return socket_cliente;


// cria socket
// liga ao servidor
// retorna fd – ok, <0 – erro
}

int Disconnect (fd)
{
int c = close(fd);

if(c<0)
	return -1;

return 1;

// fecha / destroi socket
// retorna >0 – ok, <0 – erro
}

