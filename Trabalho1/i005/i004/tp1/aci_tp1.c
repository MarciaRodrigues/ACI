/* Library */

//Sockets
#include <sys/types.h> // data types
#include <sys/socket.h> // Internet Protocol family
#include <netinet/in.h> // Internet address family
#include <pthread.h> // threads
#include <signal.h>
//Commons
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
//Serial Communication
#include "serial_util.h"
#include "simples_lista_ligada.h"
/* Define */

int serialfd = 0, MainSocket=0, ModoAux=1;
pthread_mutex_t mutex, mutex2;
pthread_t thread;
lista *lst;
char *IPaddress = NULL;

/* Functions */

int socketServer(int TCPPort); //open socket server and create threads
int socketClient(int TCPPort, char *IPaddress);
void* mode_one(void* args); //mode 1
int mode_two(int TCPPort, char* IPaddress); //mode 2
unsigned char* TCPtoASCII(int *Tid, unsigned char *TCPframe, int *length); //convert TCP frame in ASCII frame
void ASCIItoTCP(unsigned char *ASCIIframe, unsigned char *newFrame, int Tid); //convert ASCII frame in TCP frame
void ASCIIbytes(unsigned char ini, unsigned char *HB, unsigned char *LB); // create an ASCII byte
unsigned char TCPbytes(unsigned char HB, unsigned char LB); // create an TCP byte
int modbusExceptionHandler(unsigned char *frame, int modo, int sockfd); //check exception
unsigned char LRC(char *auchMsg, int usDataLen, int ini); // the function returns the LRC as a type unsigned char, quantity of bytes in message
void intHandler();

/* Main */

int main(int argc, char **argv)
{
	char SerialPort[11] = "/dev/ttyS", SerialPortNumberString[4];
	
	int SerialPortNumber = 1, TCPPort = 502, i, mode = 1;
		
	signal(SIGINT, intHandler);
	printf("***Gateway Modbus Protocol***\r\n");
	
	//Input Parameters Analysis
	for(i=1;i<argc;i++)
	{
		//TCP Port
		if(strcmp("-p", argv[i])==0) 
		{
			i++;
			TCPPort=atoi(argv[i]);
			
			if(TCPPort <= 0)
			{
				printf("Error: Invalid TCP Port(-p)\r\n");
				pthread_mutex_destroy(&mutex);
				free(IPaddress);
				return 1;
			}
		}
		//Serial Port
		else if(strcmp("-c", argv[i])==0)	
		{
			i++;
			SerialPortNumber=atoi(argv[i]);
			SerialPortNumber--;
			
			if(SerialPortNumber <= 0)
			{
				printf("Error: Invalid Serial Port(-c)\r\n");
				pthread_mutex_destroy(&mutex);
				free(IPaddress);
				return 1;
			}
		}
		//Operating Mode
		else if(strcmp("-m", argv[i])==0)	
		{
			i++;
			mode=atoi(argv[i]);
			
			if(mode != 1 && mode != 2)
			{
				printf("Error: Invalid Operating Mode(-m)\r\n");
				pthread_mutex_destroy(&mutex);
				free(IPaddress);
				return 1;
			}
		}
		//IPaddress
		else if(strcmp("-s", argv[i])==0)	
		{
			i++;
			IPaddress = (char*)malloc((strlen(argv[i])+1)*sizeof(char));
			strcpy(IPaddress, argv[i]);
		}
	}
	
	//Generate TCPPort string
	sprintf(SerialPortNumberString,"%d", SerialPortNumber);
	strcat(SerialPort, SerialPortNumberString);
	
	//Operating Parameters Print
	
	if(mode == 1)
		printf("Operating Mode 1 - Master : TCP , Slave : ASCII \r\n");
	else
		printf("Operating Mode 2 - Master : ASCII , Slave : TCP \r\n");
	
	printf("Serial Port: %s\r\nTCP Port: %d\r\n",SerialPort,TCPPort);
	
	if(IPaddress==NULL && mode == 2) 
	{
		IPaddress = (char*)malloc((strlen("127.0.0.1")+1)*sizeof(char));
		strcpy(IPaddress, "127.0.0.1");
		printf("IPaddress: %s\r\n",IPaddress);
	}
	
	//Open serial port
	
	serialfd = serial_open(SerialPort);
	
	if(serialfd==-1)	
	{
		printf("Error: Connecting to serial device\r\n");
		pthread_mutex_destroy(&mutex );
		free(IPaddress);		
		return 1;
	}
	
	//Configure Serial Port
	
	serial_config(serialfd, DEF_BAUDRATE, DEF_CHAR_SIZE, DEF_STOP_BITS, DEF_PARITY );
	
	if(mode == 1)
	{
		ModoAux=1;
		socketServer(TCPPort);
	}
	else if(mode == 2)
	{
		ModoAux=2;
		mode_two(TCPPort, IPaddress);
	}
	return 0;
	
}

void* mode_one(void* args) {
	
	int Tid, ASCIIframeLength, t;
	int sockfd = *(int*)args,i;
	unsigned char InputBuff[253+7],outputBuff[253+7], *ASCIIframe, ModbusResponse[2*253+9];
	
	printf("Waiting message from client...\r\n");

	while(recv(sockfd, InputBuff, 253+7, 0) > 0)
	{	
		if((t=modbusExceptionHandler(InputBuff, 1, sockfd))==0)
			continue;	//excepcao enviada
		else if (t==-1)
			break; //ligaçao fechada ou erro de envio
		
		printf("TCP frame:\r\n");
		printf("* Transaction id: %d\r\n", ((InputBuff[0]<<8) | InputBuff[1]));
		printf("* Protocol: %d\r\n", ((InputBuff[2]<<8) | InputBuff[3]));
		printf("* Length: %d\r\n", ((InputBuff[4]<<8) | InputBuff[5]));
		printf("* Unit Identifier: %d\r\n", InputBuff[6]);
		printf("* Function: %d\r\n\n\n", InputBuff[7]);
		
		printf("Converting TCP frame to ASCII frame\r\n\n\n");
		ASCIIframe=TCPtoASCII(&Tid, InputBuff, &ASCIIframeLength);
		
		printf("ASCII frame:\r\n");	
		printf("* Header: %d\r\n", ASCIIframe[0]);
		printf("* Unit Identifier: %d\r\n", TCPbytes(ASCIIframe[1],ASCIIframe[2]));
		printf("* Function: %d\r\n\n\n", TCPbytes(ASCIIframe[3],ASCIIframe[4]));
		
		pthread_mutex_lock(&mutex);

		if(serial_write(serialfd, ASCIIframe, ASCIIframeLength)<=0) //Send Modbus Request
		{
			printf("Error writing to ASCII Server\r\n");
			break;
		}
		
		printf("Waiting for response...\r\n\n\n");
		
		if(serial_read(serialfd, ModbusResponse, 2*253+9, 1)<=0)
		{
			printf("Error reading from ASCII Server\r\n");
			break;
		}
	
		pthread_mutex_unlock(&mutex);
		
		printf("Response - ASCII frame:\r\n");	
		printf("* Header: %d\r\n", ModbusResponse[0]);
		printf("* Unit Identifier: %d\r\n", TCPbytes(ModbusResponse[1],ModbusResponse[2]));
		printf("* Function: %d\r\n\n\n", TCPbytes(ModbusResponse[3],ModbusResponse[4]));
		
		printf("Converting ASCII frame to TCP frame\r\n\n\n");
		ASCIItoTCP(ModbusResponse,outputBuff, Tid);
		
		printf("Response - TCP frame:\r\n");
		printf("* Transaction id: %d\r\n", ((outputBuff[0]<<8) | outputBuff[1]));
		printf("* Protocol: %d\r\n", ((outputBuff[2]<<8) | outputBuff[3]));
		printf("* Length: %d\r\n", ((outputBuff[4]<<8) | outputBuff[5]));
		printf("* Unit Identifier: %d\r\n", outputBuff[6]);
		printf("* Function: %d\r\n\n\n", outputBuff[7]);
		
		if(send(sockfd, outputBuff, sizeof(unsigned char)*(((outputBuff[4]<<8) | outputBuff[5]) + 6), 0  ) <= 0)
		{	
			break;
		}
		
		printf("Message completed.\r\n");
	
	}
	pthread_mutex_lock(&mutex2);
	lista_remove(lst, sockfd);
	pthread_mutex_unlock(&mutex2);
	printf("Connection closed\r\n");
	free(ASCIIframe);
	close(sockfd);
	return;

  }
  
int mode_two(int TCPPort, char* IPaddress)
{
	unsigned char ASCIIframe[2*253+9], TCPframe[253+7], TCPresponse[253+7], *ASCIIresponse;
	int SocketFD, length=0, Tid=0, t=0, i=0; 
	
	SocketFD=socketClient(TCPPort, IPaddress);
	//printf("Waiting message from client...\r\n");
	while(1)
	{
		serial_read(serialfd, &ASCIIframe[0], 1, 1);

		if(ASCIIframe[0]!=':')
			continue;
		
		//printf("Reading...\r\n\n\n");
		
		i=0; 
		do
		{
			i++;
			serial_read(serialfd, &ASCIIframe[i], 1, 1);
		}while(ASCIIframe[i]!='\r');
		
		
		i++;
		
		serial_read(serialfd, &ASCIIframe[i], 1, 1);
		
		if(ASCIIframe[i]!='\n')
			continue;

		if((t=modbusExceptionHandler(ASCIIframe, 2, -1))==0)
			{
				ASCIIframe[0]=0;
				continue;
			}
		else if (t==-1)
		{
			printf("Error sending Exception\r\n");
			break; 
		}
		/*
		printf("ASCII frame:\r\n");	
		printf("* Header: %d\r\n", ASCIIframe[0]);
		printf("* Unit Identifier: %d\r\n", TCPbytes(ASCIIframe[1],ASCIIframe[2]));
		printf("* Function: %d\r\n\n\n", TCPbytes(ASCIIframe[3],ASCIIframe[4]));
		*/
		printf("Converting ASCII frame to TCP frame\r\n\n\n");
		ASCIItoTCP(ASCIIframe, TCPframe, Tid);
		
		/*printf("TCP frame:\r\n");
		printf("* Transaction id: %d\r\n", ((TCPframe[0]<<8) | TCPframe[1]));
		printf("* Protocol: %d\r\n", ((TCPframe[2]<<8) | TCPframe[3]));
		printf("* Length: %d\r\n", ((TCPframe[4]<<8) | TCPframe[5]));
		printf("* Unit Identifier: %d\r\n", TCPframe[6]);
		printf("* Function: %d\r\n\n\n", TCPframe[7]);*/

		if(send(SocketFD, TCPframe, sizeof(unsigned char)*(((TCPframe[4]<<8) | TCPframe[5]) + 6), 0  ) <= 0) //enviar pedido ao servidor TCP
		{
			printf("Error sending to TCP server\r\n");
			break;
		}
		
		printf("Waiting for response...\r\n\n\n");

		if(recv(SocketFD, TCPresponse, 253+7, 0)<=0) //receber resposta do servidor TCP
		{
			printf("Error receiving response from TCP server\r\n");
			break;
		}
		
		/*printf("Response - TCP frame:\r\n");
		printf("* Transaction id: %d\r\n", ((TCPresponse[0]<<8) | TCPresponse[1]));
		printf("* Protocol: %d\r\n", ((TCPresponse[2]<<8) | TCPresponse[3]));
		printf("* Length: %d\r\n", ((TCPresponse[4]<<8) | TCPresponse[5]));
		printf("* Unit Identifier: %d\r\n", TCPresponse[6]);
		printf("* Function: %d\r\n\n\n", TCPresponse[7]);
		*/
		printf("Converting TCP frame to ASCII frame\r\n\n\n");
		ASCIIresponse=TCPtoASCII(&Tid, TCPresponse, &length);
		
		/*printf("Response - ASCII frame:\r\n");	
		printf("* Header: %d\r\n", ASCIIresponse[0]);
		printf("* Unit Identifier: %d\r\n", TCPbytes(ASCIIresponse[1],ASCIIresponse[2]));
		printf("* Function: %d\r\n\n\n", TCPbytes(ASCIIresponse[3],ASCIIresponse[4]));*/

		if((length=serial_write(serialfd, ASCIIresponse, length))<=0)
			break;

		Tid++;
		ASCIIframe[0]=0;
		
		printf("Message completed.\r\n");
	}

	(void) shutdown(SocketFD, SHUT_RDWR);
	close(SocketFD);

	return 0;
}

unsigned char* TCPtoASCII(int *Tid, unsigned char *TCPframe, int *length)
{
	unsigned char *ASCIIframe;
	unsigned char aux;
	int len=0, i=0;
	
	*Tid = ((TCPframe[0]<<8) | (TCPframe[1]));
	
	len = ((TCPframe[4]<<8) | (TCPframe[5]));
	
	ASCIIframe = (unsigned char*) malloc(7+(len-1)*2); //4bytes + APDUlength - UnitIDlenfth
	ASCIIframe[0]=':';
	ASCIIbytes(TCPframe[6], &ASCIIframe[1], &ASCIIframe[2]);
	
	for(i=0; i<(len-1); i++)
	{
		ASCIIbytes(TCPframe[7+i], &ASCIIframe[3+2*i],  &ASCIIframe[4+2*i]);
	}
	
	ASCIIbytes(LRC(TCPframe, len, 6), &ASCIIframe[2*len+1], &ASCIIframe[2*len+2]); //LCR
	ASCIIframe[2*len+3]='\r'; //CR 0x0D
	ASCIIframe[2*len+4]='\n'; //LF 0x0A
	
	*length=2*len+5;
	
	return ASCIIframe;
}

void ASCIItoTCP(unsigned char *ASCIIframe, unsigned char *newFrame, int Tid)
{
	int i=0,lenght = 0;

	newFrame[0] = (0xFF00 & Tid) >> 8;
	newFrame[1] = (0xFF & Tid);
	newFrame[2] = (unsigned char)0;
	newFrame[3] = (unsigned char)0;

	newFrame[6] = TCPbytes(ASCIIframe[1], ASCIIframe[2]);

	do
	{
		newFrame[i+7] = TCPbytes(ASCIIframe[2*i+3],ASCIIframe[2*i+4]);
		lenght++;
		i++;
	}while(ASCIIframe[2*i+3]!='\r');

	newFrame[4] = (0xFF00 & lenght) >> 8;
	newFrame[5] = (0xFF & lenght);

	return;
}

void ASCIIbytes(unsigned char ini, unsigned char *HB, unsigned char *LB)
{
	unsigned char aux;
	aux=ini & 0x0F;
	if(aux>=0 && aux<=9)
		*LB=aux+'0';
	else
		*LB=aux - 10 + 'A';
	aux=ini & 0xF0;
	aux=(aux>>4);
	if(aux>=0 && aux<=9)
		*HB=aux+'0';
	else
		*HB=aux - 10 + 'A';
	return;

}

unsigned char TCPbytes(unsigned char HB, unsigned char LB)
{
	unsigned char ini = 0x00;
	if (HB>='0' && HB<='9')
	   ini = HB - '0';
	else
	   ini = HB + 10 - 'A';
	ini = (ini << 4);
	ini = ini & 0xF0;
	if (LB>='0' && LB<='9')
	   ini = ini + LB - '0';
	else
	   ini = ini + LB + 10 - 'A';
	return ini;
}

unsigned char LRC(char *auchMsg, int usDataLen, int ini) 
{	
	unsigned char uchLRC = 0 ;
	int i=0;
	for(i=0; i<(usDataLen); i++)
	{
		uchLRC += auchMsg[ini+i];
	}
	uchLRC=(0xFF-uchLRC)+1;

	return  uchLRC;

} 

int modbusExceptionHandler(unsigned char *frame, int modo, int sockfd)
{
	unsigned char *exceptionFrame, function, *auxLCR;
	int i=0;
	
	if(modo==1)
	{
		if(frame[7]!=03 && frame[7]!=16)
		{
			exceptionFrame = (unsigned char*) malloc(9*sizeof(unsigned char));
			
			for(i=0; i<4; i++)
				exceptionFrame[i]=frame[i];
			
			exceptionFrame[4]=0x00;
			exceptionFrame[5]=0x03;
			exceptionFrame[6]=frame[6];
			exceptionFrame[7]= (unsigned char)(frame[7]+0x80);
			exceptionFrame[8]=0x01;
			
			if(send(sockfd, exceptionFrame, 9*sizeof(unsigned char), 0  ) <= 0)
			{	
				free(exceptionFrame);
				return -1;
			}
			
			free(exceptionFrame);
			return 0;
		}
		else 
			return 1;
	}
	else if(modo==2)
	{
		function=TCPbytes(frame[3], frame[4]);
		if(function!=03 && function!=16)
		{
			exceptionFrame = (unsigned char*) malloc(11*sizeof(unsigned char));
			auxLCR=(unsigned char*) malloc(3*sizeof(unsigned char));
			
			for(i=0; i<3; i++)
				exceptionFrame[i]=frame[i];
			
			function=(unsigned char)(function+0x80);
			ASCIIbytes(function, &exceptionFrame[3], &exceptionFrame[4]);
			ASCIIbytes(0x01, &exceptionFrame[5], &exceptionFrame[6]);
			auxLCR[0]=TCPbytes(frame[1], frame[2]);
			auxLCR[1]=function;
			auxLCR[2]=(unsigned char)0x01;
			ASCIIbytes(LRC(auxLCR, 3, 0), &exceptionFrame[7], &exceptionFrame[8]);
			exceptionFrame[9]='\r';
			exceptionFrame[10]='\n';
			if(serial_write(serialfd, exceptionFrame, 11)<=0)
				{
					free(exceptionFrame);
					free(auxLCR);
					return -1;
				}
			free(exceptionFrame);
			free(auxLCR);
			return 0;
		}
		else
			return 1;
	}
	else
	return -1;
}

int socketServer(int TCPPort)
{
	/* Estruturas de dados */
	int sockfd, newsockfd, res; 
	socklen_t client_addr_length;
	struct sockaddr_in serv_addr, cli_addr;
	pthread_mutex_init(&mutex2 , NULL);
	pthread_mutex_init(&mutex , NULL);
	lst=lista_nova();
	/* Inicializar o socket
	 AF_INET - para indicar que queremos usar IP
	 SOCK_STREAM - para indicar que queremos usar TCP
	 sockfd - id do socket principal do servidor
	 Se retornar < 0 ocorreu um erro */
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	MainSocket=sockfd;
	if (sockfd < 0) 
	{
		printf("Error creating socket\r\n");
		return -1;
	}

	/* Criar a estrutura que guarda o endereço do servidor
	 AF_INET - endereço IP
	 INADDR_ANY - aceitar pedidos para qualquer IP */
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(TCPPort);

	/* Fazer bind do socket. Apenas nesta altura é que o socket fica ativo
	 mas ainda não estamos a tentar receber ligações.
	 Se retornar < 0 ocorreu um erro */
	res = bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
	
	if (res < 0) 
	{
		printf("Erro ao ligar-se ao socket\r\n");
		return -1;
	}

	/* Indicar que queremos escutar no socket com um backlog de 5 (podem
	 ficar até 5 ligações pendentes antes de fazermos accept */
	listen(sockfd, 5);

	while(1) 
	{
		/* Aceitar uma nova ligação. O endereço do cliente fica guardado em 
			cli_addr - endereço do cliente
			newsockfd - id do socket que comunica com este cliente */
		client_addr_length = sizeof(cli_addr);
		newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &client_addr_length);
		pthread_mutex_lock(&mutex2);
		lista_insere_inicio(lst, newsockfd);
		pthread_mutex_unlock(&mutex2);
		/* Criar uma thread para tratar dos pedidos do novo cliente */
		pthread_create(&thread, NULL, mode_one, &newsockfd);
	}

	close(sockfd);
	return 0; 
}

int socketClient(int TCPPort, char *IPaddress)
{
	struct sockaddr_in stSockAddr;
    int Res;
    int SocketFD = socket(AF_INET, SOCK_STREAM, 0);
	char aux[125];
	MainSocket=SocketFD;
	if (SocketFD < 0) {
		printf("Error creating socket\r\n");
		return -1;
    }
	memset(&stSockAddr, 0, sizeof(stSockAddr));
	stSockAddr.sin_family = AF_INET;
	stSockAddr.sin_port = htons(TCPPort);
	Res = inet_pton(AF_INET, IPaddress, &stSockAddr.sin_addr);
 
    if (0 > Res)
    {
      printf("error: first parameter is not a valid address family\r\n");
      close(SocketFD);
      return -1;
    }
    else if (0 == Res)
    {
      printf("char string (second parameter does not contain valid ipaddress)\r\n");
      close(SocketFD);
      return -1;
    }
	
	if(connect(SocketFD, (struct sockaddr *)&stSockAddr, sizeof(stSockAddr))<0)
	{
	  printf("connection failed");
	  close(SocketFD);
	  return -1;
	}
	
	return SocketFD;
}


void intHandler() 
{
	elemento *aux;
	(void) shutdown(MainSocket, SHUT_RDWR);
	close(MainSocket);
	printf("Shutting Down....\r\n");
	if(ModoAux==1)
	{
		aux=lst->raiz;
		while(aux!=NULL)
		{
			shutdown(aux->valor, SHUT_RD);
			aux = aux->proximo;
		}
		pthread_join(thread, NULL);
		printf("All clients disconnected\r\n");
		pthread_mutex_destroy(&mutex );
		pthread_mutex_destroy(&mutex2);
		lista_apaga(lst);
	}
	serial_close(serialfd);
	free(IPaddress);
	exit(1);
}