int Send_Modbus_Request(int fd, char* APDU, unsigned short nAPDU, unsigned char* APDU_R);

int Receive_Modbus_Request(int fd, char* APDU_P, unsigned short nAPDU, int* TI);

int Send_Modbus_Response(int fd, char* APDU_R, unsigned short nAPDU_R, int TI); 