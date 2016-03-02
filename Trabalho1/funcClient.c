#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <mb_c_tcp.h>
 
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

int Write_multiple_coils (fd,sy_c,n_c,val){
	
	Send_Modbus_request(fd,APDU,APDU_R);
	
	
	
}

int Read_coils (fd,st_c, n_c, val){
	
	Send_Modbus_request(fd,APDU,APDU_R);
	
}
