#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "modbustcp.h" 
#include <unistd.h>

void main(){
	system("clear"); // apaga linha de comandos para ficar um ambiente mais "Lean"
	
	char x; 
	char APDU[1000],APDU_R[1000];
	int fd=1000,teste=0, nAPDU=5;
	
	/* Debug */
	
	APDU[0]='O';
	APDU[1]='L';
	APDU[2]='A';
	APDU[3]='!';
	APDU[4]='?';
	
	while(1){
		printf("\n O que pretende fazer?\n\n Correr funçoes?(y)\n Exit(n) \n\n");
		scanf("%c", &x);
		if(x=='n')
		{
			printf("\n Bye Bye!!\n");
			break;
		}
		if (x=='y')
		{
			teste=Send_Modbus_Request(fd,APDU,nAPDU,APDU_R);
			if(teste==-1)
			{
				printf("\n Erro no Send_Modbus_Request \n");
				break;
			}
			else if(teste==1)
			{
				printf("\n Send_Modbus_Request correcto \n");
			}
		}
		else
		{
			printf("\n Introduza uma letra correcta\n\n\n");
		}
		
		
	}
}



