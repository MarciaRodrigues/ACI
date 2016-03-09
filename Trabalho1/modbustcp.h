int Send_Modbus_Request(int fd, char* APDU, int nAPDU, char* APDU_R);

int Receive_Modbus_Request(int fd, char* APDU_P, int nAPDU, int* TI);

int Send_Modbus_Response(int fd, char* APDU_R, int nAPDU_R, int TI); 