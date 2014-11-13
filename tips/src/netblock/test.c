#include <arpa/inet.h>
#include <stdio.h>

int main()
{
   unsigned int p=ntohl(3232236238);
   char buffer[32]={0};
	 inet_ntop(AF_INET,&p,buffer,32);
	 printf("%s\n",buffer);
}

