//
//  ACI_cliente_servidor.c
//  
//
//  Created by Joana Cubal Almeida on 22/02/16.


// CLIENT
//
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MYPORT 5000
#define BUFF_LENG 1024

int main()

{

    int s;
    struct sockaddr_in servidor_addr; //guardar inf que preciso relativamente ao servidor, porta...
    char buff[BUFF_LENG]; // quando recebo a mensagem vai ser guardada nesta variavel


    s= socket(PF_INET, SOCK_STREAM,0); // sock_stream - circuito virtual
    
    if (s== -1)  //para ver se o socket foi criado com sucesso
    {
        printf("ERRO");
        return -1;
    }
    puts("BEM SUCEDIDO");


    servidor_addr.sin_family = AF_INET; //AF_INET - dominio do socket (host e porta)
    servidor_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // servidor so recebe deste
    servidor_addr.sin_port = htons (MYPORT); //porta
    
    connect(s, (struct sockaddr *) &servidor_addr, sizeof (servidor_addr));
    
    
    while (1) //comunicar com o servidor
        
        {
            scanf("%s" , buff);
            
            write(s,buff,strlen(buff));
            memset(buff,0,strlen(buff));
            read(s,buff,BUFF_LENG);
            
            printf("Mensagem do eco: %s \n" , buff);
                   
                   if (buff[0]== '#')
                   break;
        
        }
                   
                   
    return 0;
                   
}