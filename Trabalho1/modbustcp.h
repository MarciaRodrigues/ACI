int Send_Modbus_request(int fd, char* APDU, char* APDU_R);

int Receive_Modbus_Request(int fd, char* APDU_P, int* TI);

int Send_Modbus_response(int fd, char* APDU_R, int TI); 