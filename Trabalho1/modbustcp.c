#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>

// CLIENTE !!!! 

int Send_Modbus_request (int fd, char* APDU, char* APDU_R) {
	
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
	
	for(i=0;i++;i<12){	
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
	}
}

// SERVIDOR !!!! 

int Receive_Modbus_request (int fd, char* APDU_P, int* TI) {
	
	if(sizeof(APDU_P)<5){
		printf("ERRO\n");
		return -1;
	}
	
	char PDU[253];
	int i,test=0;
	
	test=read(fd,PDU,sizeof(PDU),0);
	if(test<0){
		printf("ERRO");
		return -1;
	}
	
	TI=(PDU[0]*256)+PDU[1];
	
	for(i=0;i++;i<5){	
		APDU_P[i]=PDU[i+7];
		printf("APDU_P[%d]= %s\n",i,APDU_P[i]);
	}
	return 1;
}

int Send_Modbus_response (int fd, char* APDU_R, int TI) {
	
	int lenght_h=0,lenght_l=0,ui=(rand()%256),i,test=0;
	char PDU[253];
	
	lenght_h=(sizeof(PDU[6]+PDU[7]+PDU[8]+PDU[9]+PDU[10]+PDU[11]))/256;
	lenght_l=(sizeof(PDU[6]+PDU[7]+PDU[8]+PDU[9]+PDU[10]+PDU[11]))%256;
	
	PDU[0]= (unsigned char) (TI/256);
	PDU[1]= (unsigned char) (TI%256);
	PDU[2]= 0;
	PDU[3]= 0;
	PDU[4]= lenght_h;
	PDU[5]= lenght_l;
	PDU[6]= ui;
	PDU[7]= APDU_R[0];
	PDU[8]= APDU_R[1];
	PDU[9]= APDU_R[2];
	PDU[10]= APDU_R[3];
	PDU[11]= APDU_R[4];
	
	for(i=0;i++;i<12){	
		printf("PDU[%d]= %s\n",i,PDU[i]);
	}
	
	test=write(fd,PDU,strlen(PDU)+1,0);	
	if(test<0){
		printf("ERRO");
		return -1;
	}
	return 1;
}

