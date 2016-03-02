#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
 
int Receive_Modbus_request (fd, APDU_P, TI)
{
	read(fd,PDU);
}

int Send_Modbus_response (fd, APDU_R, TI)
{
	write(fd,PDU);
	
}
