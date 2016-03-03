#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>

// CLIENTE !!!!

int Send_Modbus_request (int fd, char* APDU, char* APDU_R){
	
	int r=(rand()%65000),lenght_h=0,lenght_l=0;
	char PDU[253];
	lenght_h=(strlen(PDU[5]+PDU[6]+PDU[7]+PDU[8]+PDU[9]+PDU[10])/256);
	lenght_l=(strlen(PDU[5]+PDU[6]+PDU[7]+PDU[8]+PDU[9]+PDU[10])%256);
	
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
	
	write(fd,PDU);
	read(fd,PDU_R);
}

// SERVIDOR !!!! 

int Receive_Modbus_request (int fd, char* APDU_P, int* TI) {
	
	if(sizeof(APDU_P)<5){
		printf("ERRO!");
		return -1;
	}
	char PDU[253];
	read(fd,PDU);
	TI=(PDU[0]*256)+PDU[1];
	
	for(i=0;i++;i<5){	
		APDU_P[i]=PDU[i+7];
	}
	return 1;
}

int Send_Modbus_response (int fd, char* APDU_R, int TI) {
	
	int lenght_h=0,lenght_l=0,ui=(rand()%256);
	char PDU[253];
	lenght_h=(strlen(PDU[5]+PDU[6]+PDU[7]+PDU[8]+PDU[9]+PDU[10])/256);
	lenght_l=(strlen(PDU[5]+PDU[6]+PDU[7]+PDU[8]+PDU[9]+PDU[10])%256);
	
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
	
	write(fd,PDU);	
	
	return 1;
}

