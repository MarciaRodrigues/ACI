#include <sys/types.h>
#include <sys/signal.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <sys/errno.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>


int main()
{
	int s;
	char str[1024];

	struct sockaddr_in servidor_addr;
	s= socket(PF_INET, SOCK_STREAM,0);

	if (s == -1)
	{
		printf("ERRO");
		return -1;
	}

	servidor_addr.sin_family = AF_INET;
	servidor_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	servidor_addr.sin_port = htons (1024);

	connect(s, (struct sockaddr *) &servidor_addr, sizeof (servidor_addr));

	while(1)
	{
		scanf("%s" , str);
		write(s,str,strlen(str));
		read(s,str,1024);

		printf("echo: %s \n" , str);
	
		if ( str[0] == '#')
		return 0;
	}
	
}



