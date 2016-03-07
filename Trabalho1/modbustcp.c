#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>

// CLIENTE !!!! 
int Send_Modbus_Request (int fd, char* APDU, int nAPDU, char* APDU_R) {
	
<<<<<<< HEAD
	int r=0,lenght_h=0,lenght_l=0,i=0,test=0;
	time_t t;
	char PDU[253],PDU_R[253];
	srand((unsigned) time(&t)); // inicializar o tempo para o rand
	r=(rand()%65000);
=======
	int r=(rand() % 65000),lenght_h=0,lenght_l=0,i,test=0;
	char PDU[253],PDU_R[253];
	
	lenght_h=(sizeof(PDU[6]+PDU[7]+PDU[8]+PDU[9]+PDU[10]+PDU[11]))/256;
	lenght_l=(sizeof(PDU[6]+PDU[7]+PDU[8]+PDU[9]+PDU[10]+PDU[11]))%256;
>>>>>>> origin/master
	
	PDU[0]= (unsigned char) (r/256);
	PDU[1]= (unsigned char) (r%256);
	PDU[2]= 0;
	PDU[3]= 0;
	PDU[4]= (unsigned char)((nAPDU+1)/256); //nAPDU e o tamanho do APDU e adicionamos um por causa do PDU[6] *ver capitulo livro*
	PDU[5]= (unsigned char)((nAPDU+1)%256);
	PDU[6]= 1;
	PDU[7]= APDU[0];
	PDU[8]= APDU[1];
	PDU[9]= APDU[2];
	PDU[10]= APDU[3];
	PDU[11]= APDU[4];
<<<<<<< HEAD

	printf("\n Estou dentro do Send_Modbus_Request \n\n"); // debug
	for(i=0;i<12;i++){	
	printf("PDU[%d]= %c\n",i,PDU[i]);// debug
=======
	
	for(i=0;i++;i<12){	
		printf("PDU[%d]= %s\n",i,PDU[i]);
>>>>>>> origin/master
	}
	
	test=write(fd,PDU,strlen(PDU)+1);
	if(test<0){
<<<<<<< HEAD
		printf("\n ERRO1\n");
=======
		printf("ERRO\n");
>>>>>>> origin/master
		return -1;
	}
	
	test=read(fd,PDU_R,sizeof(PDU_R));
	if(test<0){
<<<<<<< HEAD
		printf("\n ERRO2\n");
=======
		printf("ERRO\n");
>>>>>>> origin/master
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

