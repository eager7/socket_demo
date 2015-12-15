#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <unistd.h> //usleep

#define checkError(ret) do{if(-1==ret){printf("[%d]err:%s\n", __LINE__, strerror(errno));exit(1);}}while(0)

int main(int argc, char const *argv[])
{
	printf("udp broadcast client\n");

	int iSocketFd = socket(AF_INET, SOCK_DGRAM, 0);
	checkError(iSocketFd);

	int on = 1;
	int broadcastEnable = 1;//the permissions of broadcast
	checkError(setsockopt(iSocketFd, SOL_SOCKET, SO_BROADCAST, (char *)&broadcastEnable, sizeof(broadcastEnable)));
	checkError(setsockopt(iSocketFd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on)));

	struct sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family 	= AF_INET; 
	server_addr.sin_port 	= htons(18020);
	server_addr.sin_addr.s_addr = htonl(INADDR_BROADCAST);

	char aRecv[2048] = {0};
	const char *aSend = "This is udp brocast client";	
	while(1)
	{
		printf("send hello to server\n");
		checkError(sendto(iSocketFd, aSend, strlen(aSend), 0, (struct sockaddr*)&server_addr, sizeof(server_addr)));

		sleep(1);
	}
	
	return 0;
}
