#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
 
int Send_Modbus_request (fd, APDU, APDU_R)
{
	write(fd,PDU);
	read(fd,PDU_R);
	
}

