#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
// #include "modbustcp.h"

void main(){
	char fd=0, teste=0;
	char APDU[1000],APDU_R[1000];
	
	Send_Modbus_Request(fd,&APDU,&APDU_R);
	
	printf("%d\n",APDU);
	printf("%d\n",APDU_R);
}




int Send_Modbus_Request (int fd, char* APDU, char* APDU_R) {
	
	int r=(rand() % 65000),lenght_h=0,lenght_l=0,i,test=0;
	char PDU[253],PDU_R[253];
	
	lenght_h=(sizeof(PDU[6]+PDU[7]+PDU[8]+PDU[9]+PDU[10]+PDU[11]))/256;
	lenght_l=(sizeof(PDU[6]+PDU[7]+PDU[8]+PDU[9]+PDU[10]+PDU[11]))%256;
	
	PDU[0]= (unsigned char) (r/256);
	PDU[1]= (unsigned char) (r%256);
	PDU[2]= 0;
	PDU[3]= 0;
	PDU[4]= lenght_h;
	PDU[5]= lenght_l;
	PDU[6]= 1;
	PDU[7]= APDU[0];
	PDU[8]= APDU[1];
	PDU[9]= APDU[2];
	PDU[10]= APDU[3];
	PDU[11]= APDU[4];
	
	/* for(i=0;i++;i<12){	
		printf("PDU[%d]= %s\n",i,PDU[i]);
	}
	
	test=write(fd,PDU,strlen(PDU)+1,0);
	if(test<0){
		printf("ERRO\n");
		return -1;
	}
	
	test=read(fd,PDU_R,sizeof(PDU_R),0);
	if(test<0){
		printf("ERRO\n");
		return -1;
	} */
	
}