#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
 
int main()
{
	int Sclient=0, Cclient=0, Wclient=0, Rclient=0;
	char send[100];
	char receive[100];
	struct sockaddr_in addr;
	
	bzero( &addr ,sizeof(addr));
	
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	addr.sin_port = htons(22000);
		
	Sclient=socket(PF_INET,SOCK_STREAM,0);
	
	if(Sclient<0)
		printf("ERRO ao criar socket");
	
	Cclient= connect(Sclient, (struct sockaddr*) & addr, sizeof (addr));
	printf("Conexão iniciada, envie a sua mensagem.\n\n");
	
	if(Cclient<0)
		printf("ERRO ao conectar ao servidor");
	
	while(1)
	{
		bzero(send,sizeof(send));
		bzero(receive,sizeof(receive));
		fgets(send,sizeof(send),stdin);
		Wclient=write(Sclient,send,strlen(send)+1,0);
		if(Wclient<0){
			printf("ERRO ao enviar para o servidor");
			break;
		}
		Rclient=read(Sclient,receive,sizeof(receive),0);
		if(Rclient<0){
			printf("ERRO ao receber do servidor");
			break;
		}
		printf("Servidor recebeu a sua mensagem: %s\n", receive);
	}
}
	

	
	

