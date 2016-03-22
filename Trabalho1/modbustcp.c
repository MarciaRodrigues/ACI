#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>

// CLIENTE !!!! 
int Send_Modbus_Request (int fd, unsigned char* APDU, unsigned short nAPDU, unsigned char* APDU_R) {
	
	int r=0,i,random=0,w=0;
	time_t t;
	unsigned char PDU[260],PDU_R[260];
	srand((unsigned) time(&t)); // inicializar o tempo para o rand
	random=(rand()%65000);
	
	bzero(PDU,sizeof(PDU)); //zerar o buffer
	bzero(PDU_R,sizeof(PDU_R)); //zerar o buffer
	
	PDU[0]= (unsigned char) (random/256);
	PDU[1]= (unsigned char) (random%256);
	PDU[2]= 0;
	PDU[3]= 0;
	PDU[4]= (unsigned char)((nAPDU+1)/256); //nAPDU é o tamanho do APDU e adicionamos um por causa do PDU[6] *ver capitulo livro*
	PDU[5]= (unsigned char)((nAPDU+1)%256);
	PDU[6]= 1;
	
	for ( i = 0; i < nAPDU; i++){
		PDU[i+7] = APDU[i];
	}
	
	for ( i = 0; i < nAPDU+7; i++){
		printf("PDU[%d]= %02x\n",i,PDU[i]);
	}
		
	//printf("PDU[%d]= %02x\n",nAPDU+8,PDU[nAPDU+8]);
	
	w=send(fd,PDU,nAPDU+7,0);
	printf("\n Fiz write \n");
	
	r=recv(fd,PDU_R,sizeof(PDU_R),0);
	printf("\n Fiz read \n");
	
	for(i=0;i<nAPDU+7;i++){
		printf("PDU_R[%d]= %02x\n",i,PDU_R[i]);
	}
	
	if(w<0){ // testa se o valor retornado e igual ao numero de coils que efectivamente escreveu
		printf("\n ERRO1\n");

		return -1;
	}
	else if(r<0){ // testa se o valor retornado pela funçao e igual ao numero de coils que efectivamente leu
		printf("\n ERRO2\n");
		return -1;
	}
	
	for(i=0;i<18;i++){
		APDU_R[i]=PDU_R[i]; // preenche APDU_R
	}
	
	/*for(i=0;i<18;i++){
		printf("APDU_R[%d]= %d\n",i,APDU_R[i]);
	}*/
	
	
	
	
	return 1;
	
}

// SERVIDOR !!!! 

int Receive_Modbus_Request (int fd, char* APDU_P, unsigned short nAPDU_P, int* TI) {
	
	if(sizeof(APDU_P)<5){
		printf("ERRO\n");
		return -1;
	}
	
	char PDU[253];
	int i,test=0;
	
	if((read(fd,PDU,sizeof(PDU)))<0){
		printf("ERRO");
		return -1;
	}

	*TI=(PDU[0]*256)+PDU[1];
	
	for(i=0;i++;i<5){	
		APDU_P[i]=PDU[i+7];
		printf("APDU_P[%d]= %s\n",i,APDU_P[i]);
	}
	return 1;
}

int Send_Modbus_Response (int fd, char* APDU_R, unsigned short nAPDU_R, int TI) {
	
	time_t t;
	srand((unsigned) time(&t)); // inicializar o tempo para o rand
	
	int lenght_h=0,lenght_l=0,ui=(rand()%256),i,test=0;
	char PDU[253];
	
	PDU[0]= (unsigned char) (TI/256);
	PDU[1]= (unsigned char) (TI%256);
	PDU[2]= 0;
	PDU[3]= 0;
	PDU[4]= (unsigned char)((nAPDU_R+1)/256); //nAPDU_R e o tamanho do APDU_R e adicionamos um por causa do PDU[6] *ver capitulo livro*
	PDU[5]= (unsigned char)((nAPDU_R+1)%256);
	PDU[6]= ui;
	PDU[7]= APDU_R[0];
	PDU[8]= APDU_R[1];
	PDU[9]= APDU_R[2];
	PDU[10]= APDU_R[3];
	PDU[11]= APDU_R[4];
	
	for(i=0;i++;i<12){	
		printf("PDU[%d]= %s\n",i,PDU[i]); // debug
	}

	if((write(fd,PDU,strlen(PDU)+1))<0){
		printf("ERRO");
		return -1;
	}
	return 1;
}
