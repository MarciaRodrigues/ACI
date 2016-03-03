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

char str[1024];
struct sockaddr_in laddr;
int s1, s2,len, l;

s1=socket(PF_INET,SOCK_STREAM,0);
if (s1 == -1)
{
	printf("ERRO");
	return -1;
}

//int inet_aton(const char *cp, struct in_addr *inp);
laddr.sin_family=AF_INET;
laddr.sin_port=htons(1024);
laddr.sin_addr.s_addr=INADDR_ANY;


puts("soccket bem sucedida");



l=bind(s1,(const struct sockaddr*)&laddr, sizeof(laddr));

listen(s1,5);

s2=accept(s1,NULL, NULL);

if (s2<0)
{
	printf("ERRO");
	return -1;
}

while(1)
{

	len=read(s2,str,100);
	puts("recebido");
	write(s2,str,len);

	if(str[0] == '#')
	break;
}

close(s2);
return 0;

}