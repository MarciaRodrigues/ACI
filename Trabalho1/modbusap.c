#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <modbustcp.h>
 
int cConnect (server_add, port){
	
	int Sclient=0, Cclient=0, Wclient=0, Rclient=0;
	char send[100];
	char receive[100];
	struct sockaddr_in addr;
	
	bzero( &addr ,sizeof(addr));
	
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = server_add;
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
	return Cclient;
	
}

int cDisconnect (fd){
	close(fd);
	
	/* if(fd==NULL){
		return 1;
	}				!!!!!!!!!!!!!!! Nao tenho a certeza !!!!!!!!!!!!!!!
	else{
		return -1;
	}*/
	
}

int Write_multiple_coils (int fd, int st_c, int n_c, char *val){
	
	if(n_c>0){
		printf("ERRO!!");
		return -1;
	}

	unsigned char APDU[253], APDU_R[253];
	APDU[0]=15;// porque e write multiple coils;
	
	APDU[1]=(unsigned char)(st_c/256);
	APDU[2]=(unsigned char)(st_c%256);
	APDU[3]=(unsigned char)(n_c/256);
	APDU[4]=(unsigned char)(n_c%256);
	
	Send_Modbus_request(fd,APDU,APDU_R);
	
}

int Read_coils (int fd, int st_c, int n_c, char *val){
	
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
	
	
	Send_Modbus_request(fd,APDU,APDU_R);
	
}
