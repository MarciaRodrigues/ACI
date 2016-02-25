#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
 
int main()
{
	int Sserver=0, Aserver=0, Wserver=0, Rserver=0, tbind=0, tlisten=0;
	char msg[100];
	struct sockaddr_in addr;
	
	bzero(&addr ,sizeof(addr));
	
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY; 
	addr.sin_port = htons(22000);
		
	Sserver=socket(PF_INET,SOCK_STREAM,0);
	
	if(Sserver<0)
		printf("ERRO ao criar socket");
	
	tbind = bind(Sserver,(struct sockaddr*) & addr, sizeof (addr));
	tlisten=listen(Sserver,1);
	
	if((tbind<0)||(tlisten<0))
		printf("ERRO");
	
	Aserver=accept(Sserver,(struct sockaddr*) NULL, NULL);
	if(Aserver<0)
		printf("ERRO ao conectar");
	
	printf("Um cliente conectou-se.\n\n");
	
	while(1)
	{
		bzero(msg,sizeof(msg));
		Rserver=read(Aserver,msg,sizeof(msg),0);
		if(Rserver<0){
			printf("ERRO ao receber do cliente");
			break;
		}
		
		printf("Mensagem do cliente: %s\n", msg);
	
		Wserver=write(Aserver,msg,strlen(msg)+1,0);
		if(Wserver<0){
			printf("ERRO");
			break;
		}
	}
}


