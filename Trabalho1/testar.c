#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "modbusap.h"
// #include "modbustcp.h" 
#include <unistd.h>
#include <arpa/inet.h>

void main(){
	
	char x;
	char APDU[255],APDU_R[255];

	int fd=0,teste=0, nAPDU=5;
	
	/* Debug */
	
	APDU[0]='O';
	APDU[1]='L';
	APDU[2]='A';
	APDU[3]='!';
	APDU[4]='?';
	
	system("clear");
	
	printf("\n O que pretende fazer?\n\n Correr funçoes?(y)\n Exit(n) \n\n");
	
	while(1){
		
		scanf("%c", &x);
		if(x=='n')
		{
			printf("\n Bye Bye!!\n");
			break;
		}
		if (x=='y')
		{
			
			fd=cConnect(inet_addr("127.0.0.1"),522);
			
			if(fd==-1)
			{
				printf("\n Erro ao connectar \n");
				break;
			}
			else if(fd==1)
			{
				printf("\n Connected \n");
				printf("\n Proximo teste?\n\n");
			}
		}
		else
		{
			printf("\n Introduza uma letra correcta\n\n\n");
		}
		printf("\n O que pretende fazer?\n\n Correr funçoes?(y)\n Exit(n) \n\n");
		
	}

}