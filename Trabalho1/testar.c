#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <modbustcp.h> 

void main(){
	char x=0;
	char APDU[1000],APDU_R[1000];
	int fd=1000,teste=0;
	while(1){
		printf("O que pretende fazer?\n Correr funçoes?(y)\n Exit(n) \n");
		scanf("%c", &x);
		if(x=='n'){
			printf("Bye Bye!!\n");
			break;
			}
		else if((x!='n') || (x!='y')){
			printf("Introduza uma letra correcta\n\n\n");
		}
		else if (x=='y'){
			printf("Vamos executar Send_Modbus_Request!!\n\n\n");
			//teste=Send_Modbus_Request(fd,APDU,APDU_R);
			if(teste==-1){
				printf("Erro no Send_Modbus_Request \n");
				break;
			}
		}
	}
}
