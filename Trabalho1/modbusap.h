int cConnect (int server_add, int port);
int cDisconnect (int fd);
int Write_multiple_coils (int fd, unsigned short startAddress, unsigned short nCoils, unsigned char *val);
int Read_coils (int fd, unsigned short startAddress, unsigned short nCoils, unsigned char *val);
int sConnect(int port);
