//
//  ACI_servidor_cliente.c
//servidor
//
//  Created by Joana Cubal Almeida on 22/02/16.
//
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MYPORT 5000
#define BUFF_LENG 1024

int main()
{
    
    int s;
    int socket_cliente;
    struct sockaddr_in servidor; //guardar inf que preciso relativamente ao servidor, porta...
    char buff[BUFF_LENG]; // quando recebo a mensagem vai ser guardada nesta variavel
    
    servidor.sin_family = AF_INET; //AF_INET - dominio do socket (host e porta)
    servidor.sin_addr.s_addr = INADDR_ANY; // servidor pode receber de qualquer indereço
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
    
//listen - numero de ligaçoes que o teu servidor consegue manter
    
    listen(s,5);
    
    socket_cliente = accept (s, NULL, NULL);
    
        if (socket_cliente<0)
            
        {
            printf("ERRO");
            
            return -1;
        }
            
     while(1) //receber e enviar ao cliente
         
     {
         
    //memset - limpar o buff -> como estas sempre a receber mensagem tens que ir eliminado as coisas
         
    memset(buff,0,strlen(buff));
    read(socket_cliente, buff, BUFF_LENG -1); //no read recebes "lixo"
    
    puts ("RECEBIDA");
    write(socket_cliente,buff, strlen(buff));
         
    if(buff[0] == '#')
        break;
    
    }
    
    close(socket_cliente);
    
    return 0 ;
    
            
            
        
}