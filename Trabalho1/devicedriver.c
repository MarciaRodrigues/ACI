#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include "modbustcp.h"
#include <arpa/inet.h>
#include <math.h>  



// SERVIDOR !!!! 


int R_coils (unsigned short startAddress, unsigned short nCoils, unsigned char *val){
	
	int Rserver=0;
	
	// Read nCoils from startAddress
	
	
	// Write val
	
	
}



int W_coils (unsigned short startAddress, unsigned short nCoils, unsigned char *val){
	
	// Write nCoils from startAddress
	
	
	// Write val
}