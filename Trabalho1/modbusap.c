#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include "modbustcp.h"
#include <arpa/inet.h>

int cConnect (int server_add, int port){
	
	int Sclient=0, Cclient=0, Wclient=0, Rclient=0;
	char send[100];
	char receive[100];
	struct sockaddr_in addr;
	
	bzero( &addr ,sizeof(addr));
	
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	addr.sin_port = port;
		
	Sclient=socket(PF_INET,SOCK_STREAM,0);
	
	if(Sclient<0) {
		printf("ERRO ao criar socket");
		return -1;
	}
		
	Cclient= connect(Sclient, (struct sockaddr*) & addr, sizeof (addr));
	
	if(Cclient<0) {
		printf("ERRO ao conectar ao servidor");
		return -1;
	}
	printf("\n\nConnectado!!\n\n");
	return Cclient;
	
}

int cDisconnect (int fd){

	if (close(fd)<0)
	{
		printf("error close");
		return -1;
	}
	else
	{
		return 0;
	}
	
}

int Write_multiple_coils (int fd, unsigned short startAddress, unsigned short nCoils, unsigned char *val){
	
	unsigned char *APDU, *APDU_R;
	unsigned short N, Req;
	
	// Calculate total bytes
	N = ceil(nCoils/8.0);

	APDU = new unsigned char[N+6];

	// Function Code
	APDU[0] = '15';

	// Start Address
	APDU[1] = (unsigned char) (startAddress/256);
	APDU[2] = (unsigned char) (startAddress%256);

	// Number of Coils
	APDU[3] = (unsigned char) (nCoils/256);
	APDU[4] = (unsigned char) (nCoils%256);

	// Byte Count
	APDU[5] = N;

	// Coil Values
	for (int i = 0; i < N; i++)
		APDU[i+6] = coilValues[i];

	APDU_R = new unsigned char[5];
	
	for(nAPDU=0;nAPDU<=nCoils;nAPDU++){
		APDU[nAPDU]=val[nAPDU];
	}

	// Send Request
	Req = Send_Modbus_Request(fd, APDU, nAPDU, &APDU_R);

	if (Req < 0){ 
		printf("erro");
		return -1;
	}
		
	if (APDU_R[0] == 0x8F){ 
		printf("erro");
		return -1;
	}

	return N;
	
	
	/*
	int smr=0,nAPDU;
	char* APDU[253],APDU_R[253];
	
	APDU[0]= '15'; // porque e write multiple coils
	
	APDU[1]= (unsigned char) (startAddress/256);
	APDU[2]= (unsigned char) (startAddress%256);
	
	APDU[1]= (unsigned char) (nCoils/256);
	APDU[2]= (unsigned char) (nCoils%256);
	
	APDU[3]= '3';
	
	for(nAPDU=0;nAPDU<=nCoils;nAPDU++){
		APDU[nAPDU]=val[nAPDU];
	}
	
	smr = Send_Modbus_Request(fd, APDU, nAPDU, &APDU_R);
	if (smr<0)
	{
		printf("erro");
		return -1;
	}
	else
	{
		return n_c;
	}
	
	*/
	
}

int Read_coils (int fd, unsigned short startAddress, unsigned short nCoils, unsigned char **val){
	
	
	unsigned char *APDU, *APDU_R;
	unsigned short N, Req;

	PDU = new unsigned char[5];

	// Function Code
	APDU[0] = 1;

	// Start Address
	APDU[1] = (unsigned char) (startAddress/256);
	APDU[2] = (unsigned char) (startAddress%256);

	// Number of Coils
	APDU[3] = (unsigned char) (nCoils/256);
	APDU[4] = (unsigned char) (nCoils%256);

	// Calcule response maximum size
	N = ceil(nCoils/8.0);

	APDU_R = new unsigned char[N+2];

	Req = Send_Modbus_Request(fd, APDU, nAPDU, &APDU_R);
	
	if (Req < 0) {
		printf("erro");
		return -1;
	}
	
	if (APDU_R[0] == 0x81){
		printf("erro");
		return -1;
	}
		

	// Write Coil Values
	*coilValues = new unsigned char[N];
	
	for (int i = 0; i < N; i++)
		(*coilValues)[i] = PDU_R[i+2];

	return N;
	
	
	
	/*
	if(n_c>0){
		printf("ERRO!!");
		return -1;
	}
	
	unsigned char APDU[253], APDU_R[253];
	APDU[0]=1;// porque e read coils;
	
	APDU[1]=(unsigned char)(st_c/256);
	APDU[2]=(unsigned char)(st_c%256);
	APDU[3]=(unsigned char)(n_c/256);
	APDU[4]=(unsigned char)(n_c%256);
	
	
	Send_Modbus_Request(fd,APDU,APDU_R);
	*/
}


//SERVIDOR

int Request_handler(int fd) {
	
	int TI;
	char APDU_P[253];
	int nAPDU_R=25;
	int rmr;
	rmr = Receive_Modbus_Request(fd, &APDU_P, &TI);
	if (rmr==0)
	{
		printf("error");
	}
	
	if(APDU_P[0]=='15')
	{
		int st_c, nc;
		char val[4];
		st_c = (APDU_P[1]+APDU_P[2]/10)*256;
		nc = (APDU_P[3]+APDU_P[4]/10)*256;
		val[0]=APDU_P[6];
		val[1]=APDU_P[7];
		val[2]=APDU_P[8];
		val[3]=APDU_P[9];
		// W_coils(st_c,nc, val);			//falta implementar
		
		char APDU_R[5];
		APDU_R[0]='15';
		APDU_R[1]=APDU_P[1];
		APDU_R[2]=APDU_P[2];
		APDU_R[3]= APDU_P[3];
		APDU_R[4]=APDU_P[4];
		
	
		int smr;
		smr= Send_Modbus_Response(fd, nAPDU_R, APDU_R, &TI);
		return 0;
	}
}

int sConnect(int port) {
	
	int fd, cli_size, s2;		//socket
	fd = socket(PF_INET, SOCK_STREAM, 0);
	
	// definir estrutura do socket
	struct sockaddr_in laddr;
	struct sockaddr_in cli_laddr;
	
	laddr.sin_family = AF_INET;/*address family internet*/
	laddr.sin_port = htons(port); //porta
	laddr.sin_addr.s_addr=INADDR_ANY;
	
	cli_size=sizeof(cli_laddr);//tamanho da struct laddr
	
	bind(fd, (struct sockaddr *) &laddr, sizeof(laddr));
	
	if (listen(fd,128) == -1)
	{ 
		printf("erro listen");
		return 0;
	}
	
	s2 = accept(fd, (struct sockaddr *) &cli_laddr, &cli_size);
	
	return fd;
}
