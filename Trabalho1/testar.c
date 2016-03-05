#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <modbustcp.h> 
#include <unistd.h>

void main(){
	system("clear"); // apaga linha de comandos para ficar um ambiente mais "Lean"
	
	char x; 
	char APDU[1000],APDU_R[1000];
	int fd=1000,teste=0;
	while(1){
		printf("O que pretende fazer?\n\n Correr funçoes?(y)\n Exit(n) \n\n");
		scanf("%c", &x);
		if(x=='n')
		{
			printf("\nBye Bye!!\n");
			break;
		}
		if (x=='y')
		{
			teste=Send_Modbus_Request(fd,APDU,APDU_R);
			if(teste==-1)
			{
				printf("Erro no Send_Modbus_Request \n");
				break;
			}
			else if(teste==1)
			{
				printf("Send_Modbus_Request correcto \n");
			}
		}
		else
		{
			printf("\nIntroduza uma letra correcta\n\n\n");
		}
		
		
	}
}
