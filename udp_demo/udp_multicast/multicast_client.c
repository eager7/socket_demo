#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <unistd.h> //usleep

#define checkError(ret) do{if(-1==ret){printf("[%d]err:%s\n", __LINE__, strerror(errno));exit(1);}}while(0)

int main(int argc, char const *argv[])
{
	printf("this is udp multicast client\n");

	int iSocketFd = socket(AF_INET, SOCK_DGRAM, 0);
	checkError(iSocketFd);

	struct sockaddr_in server_addr; 
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;  
    server_addr.sin_addr.s_addr = inet_addr("239.227.227.227");
    server_addr.sin_port = htons(7878);

    char aRecv[2048] = {0};
    const char *aSend = "Hello Multicast Server";
    while(1)
    {
    	checkError(sendto(iSocketFd, aSend, strlen(aSend), 0, (struct sockaddr*)&server_addr, sizeof(server_addr)));

    	memset(aRecv, 0, sizeof(aRecv));
    	checkError(recv(iSocketFd, aRecv, sizeof(aRecv), 0));
    	printf("Recv Server Data:%s\n", aRecv);
    	sleep(1);
    }

	return 0;
}